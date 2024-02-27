#include "typelinker.h"
#include <unordered_map>
#include <set>
#include <iostream>
#include <environment-builder/symboltable.h>
#include <variant>
#include <algorithm>
#include <unordered_map>
#include "exceptions/compilerdevelopmenterror.h"
#include "utillities/util.h"
#include <functional>

using namespace std;

std::string getIdentifier(TypeDeclaration type_dec) {
    return std::visit([&](auto type) { return type->identifier; }, type_dec);
}

TypeLinker::TypeLinker(
    PackageDeclarationObject &default_package
) : 
    default_package{&default_package},
    current_package{&default_package}
{};

PackageDeclarationObject* TypeLinker::findPackageDeclaration(
    QualifiedIdentifier &qualified_identifier
) {
    PackageDeclarationObject* temp_package = default_package;

    for (auto &identifier : qualified_identifier.identifiers) {
        // Verify doesn't resolve to class not in default package
        if (!temp_package->is_default_package) {
            auto possible_class = temp_package->classes->lookupUniqueSymbol(identifier.name);
            auto possible_interface = temp_package->interfaces->lookupUniqueSymbol(identifier.name);
            if (possible_class || possible_interface) {
                auto get_type_name = [&](auto type) { return getIdentifier(util::getAsTypeDeclaration(type)); };
                auto type_name = possible_class ? get_type_name(possible_class) : get_type_name(possible_interface);
                throw SemanticError("Package prefix resolves to type " + type_name);
            }
        }

        // Resolve to subpackage
        auto possible_package = temp_package->sub_packages->lookupUniqueSymbol(identifier.name);
        if (possible_package) {
            temp_package = &(std::get<PackageDeclarationObject>(*possible_package));
        } else {
            // Reference to undeclared package
            throw SemanticError("Undeclared package: " + identifier.name + " in package " + temp_package->identifier);
        }
    }

    return temp_package;
}

TypeDeclaration TypeLinker::findTypeImport(QualifiedIdentifier &qualified_identifier) {
    PackageDeclarationObject* temp_package = default_package;

    for (auto &identifier : qualified_identifier.identifiers) {
        // Verify the qualified identifier resolves to a type at the last identifier, and the last only
        auto possible_class = temp_package->classes->lookupUniqueSymbol(identifier.name);
        auto possible_interface = temp_package->interfaces->lookupUniqueSymbol(identifier.name);

        if ((possible_class || possible_interface)) {
            if (&identifier != &qualified_identifier.identifiers.back() && current_package != default_package) {
                // This is only problematic if the possible class / interface is NOT in the default package
                throw SemanticError("Prefix of fully qualified type resolves to type");
            }
            if (possible_class) {
                return util::getAsTypeDeclaration(possible_class);
            } else if (possible_interface) {
                return util::getAsTypeDeclaration(possible_interface);
            }
            throw CompilerDevelopmentError("was not class or interface");
        }

        // Resolve to subpackage
        auto possible_package = temp_package->sub_packages->lookupUniqueSymbol(identifier.name);
        if (possible_package) {
            temp_package = &(std::get<PackageDeclarationObject>(*possible_package));
        } else {
            // Reference to undeclared package
            throw SemanticError("Undeclared package");
        }
    }
    throw SemanticError("Undeclared type imported");
}

std::optional<PackageDeclarationObject*> tryFindPackageInPackage(
    QualifiedIdentifier &qualified_identifier, 
    PackageDeclarationObject* source_package
) {
    PackageDeclarationObject* temp_package = source_package;

    for (auto &identifier : qualified_identifier.identifiers) {
        // Resolve to subpackage
        auto possible_package = temp_package->sub_packages->lookupUniqueSymbol(identifier.name);
        if (possible_package) {
            temp_package = &(std::get<PackageDeclarationObject>(*possible_package));
        } else {
            // Cannot find package
            return std::nullopt;
        }
    }

    return temp_package;
}

std::optional<TypeDeclaration> tryFindTypeInPackage(std::string &identifier, PackageDeclarationObject* package_dec) {
    auto possible_classes = package_dec->classes->lookupSymbol(identifier);
    auto possible_interfaces = package_dec->interfaces->lookupSymbol(identifier);

    if (possible_classes) {
        return &std::get<ClassDeclarationObject>(possible_classes->back());
    } else if (possible_interfaces) {
        return &std::get<InterfaceDeclarationObject>(possible_interfaces->back());
    }
    return std::nullopt;
}

TypeDeclaration resolveCandidates(std::vector<TypeDeclaration>& valid_candidates, std::string identifier) {
    util::removeDuplicates(valid_candidates);
    if (valid_candidates.size() == 0) {
        throw SemanticError("Type declaration for " + identifier + " not found");
    } else if (valid_candidates.size() > 1) {
        std::string error_message = "Type '" + identifier + "' is ambigious. Possible candidates are ";
        for (auto candidate : valid_candidates) {
            std::visit([&](auto type){ error_message += "'" + type->identifier + "'"; }, candidate);
            if (candidate != valid_candidates.back()) {
                error_message += ", ";
            }
        }
        throw SemanticError(error_message);
    }
    return valid_candidates.back();
}

TypeDeclaration TypeLinker::lookupQualifiedType(QualifiedIdentifier &qualified_identifier) {
    /*
        * Typelinking:
        * - Qualified names of the form Q.id are handled by these rules: 
        *   1. Q must be a package name in the environment.
        *   2. No prefix of Q, including Q, can be a type name.
        *   3. id must be exactly one type that is a member of Q.
    */
    QualifiedIdentifier package_qid = qualified_identifier.getQualifiedIdentifierWithoutLast();
    std::string canoncial_name = qualified_identifier.identifiers.back().name;

    if (package_qid.identifiers.empty()) {
        auto valid_candidates = lookupSimpleType(canoncial_name);
        return resolveCandidates(valid_candidates, canoncial_name);
    }

    // Check each import-on-demand package in compilation units namespace
    auto getValidCandidates = [&](QualifiedIdentifier &package_qid, std::string canoncial_name){
        std::vector<TypeDeclaration> valid_candidates;
        
        auto all_packages = star_imports;
        all_packages.push_back(default_package);
        
        for (auto package : all_packages) {
            auto possible_package = tryFindPackageInPackage(package_qid, package);
            if (possible_package) {
                if (auto possible_type = tryFindTypeInPackage(canoncial_name, *possible_package)) {
                    valid_candidates.push_back(*possible_type);
                }
            }
        }

        return valid_candidates;
    };

    // Verify 1.
    auto valid_candidates = getValidCandidates(package_qid, canoncial_name);

    // Verify 2.
    std::string conflicting_prefix;
    // Return true if any non-strict prefix of qid resolves to a type in the environment.
    std::function<bool(QualifiedIdentifier&)> somePrefixIsType = [&](QualifiedIdentifier &qid){
        auto prefix = qid.getQualifiedIdentifierWithoutLast();
        std::string canoncial_name = qid.identifiers.back().name;

        if (prefix.identifiers.empty()) {
            // This is a simple type
            conflicting_prefix = canoncial_name;
            return !lookupSimpleType(canoncial_name).empty();
        } else if (!getValidCandidates(prefix, canoncial_name).empty()) {
            conflicting_prefix = qid.getQualifiedName();
            return true;
        }
        return somePrefixIsType(prefix);
    };
    if (somePrefixIsType(package_qid)) {
        auto stringified_qid = qualified_identifier.getQualifiedName();
        throw SemanticError(
            "A strict prefix, " + conflicting_prefix 
                + ", of the fully qualified type " + stringified_qid + " resolves to a type."
        );
    }

    // Verify 3.
    return resolveCandidates(valid_candidates, canoncial_name);
}

std::vector<TypeDeclaration> TypeLinker::lookupSimpleType(std::string &identifier) {
    /*
        * Typelinking:
        * - Unqualified names are handled by these rules: 
        *   1. try the class or interface of the compilation unit, or any single-type-import (A.B.C.D) 
        *   2. try the same package, across all compilation units
        *   3. try any import-on-demand package (A.B.C.*), including java.lang.* 
    */
    std::vector<TypeDeclaration> valid_candidates;

    // 1a. Try the type declared in this compilation unit, if it exists
    std::visit([&](auto class_or_interface) {
        if (class_or_interface && class_or_interface->identifier == identifier) {
            valid_candidates.push_back(class_or_interface);
        }
    }, current_type);

    // 1b. Look up in list of all single type imports
    for (auto type_dec : single_imports) {
        std::visit([&](auto class_or_int_dec) {
            if (class_or_int_dec->identifier == identifier) {
                valid_candidates.push_back(type_dec);
            }
        }, type_dec);
    }

    if (valid_candidates.size() > 0) {
        return valid_candidates;
    }

    // 2. Look up in current package
    if (auto possible_type = tryFindTypeInPackage(identifier, current_package)) {
        valid_candidates.push_back(*possible_type);
    }

    if(valid_candidates.size() > 0) {
        return valid_candidates;
    }

    // 3. Look up in imported packages
    for (auto package_dec : star_imports) {
        auto possible_classes = package_dec->classes->lookupSymbol(identifier);
        auto possible_interfaces = package_dec->interfaces->lookupSymbol(identifier);

        if (possible_classes) {
            for (auto &possible_class : *possible_classes) {
                TypeDeclaration found_type = &std::get<ClassDeclarationObject>(possible_class);
                valid_candidates.push_back(found_type);
            }
        }
        if (possible_interfaces) {
            for (auto &possible_interface : *possible_interfaces) {
                TypeDeclaration found_type = &std::get<InterfaceDeclarationObject>(possible_interface);
                valid_candidates.push_back(found_type);
            }
        }
    }

    return valid_candidates;
}

/* Visitor implementation */

void TypeLinker::operator()(CompilationUnit &node) {
    // Import java.lang implicitly
    auto java_lang = QualifiedIdentifier(std::vector<Identifier>{Identifier("java"), Identifier("lang")});
    star_imports.emplace_back(findPackageDeclaration(java_lang));

    std::string current_type_name;

    // Get the current type name
    if (node.class_declarations.size() > 0) {
        current_type_name = node.class_declarations[0].class_name->name;
    } else if (node.interface_declarations.size() > 0) {
        current_type_name = node.interface_declarations[0].interface_name->name;
    }

     // Make package of compilation unit accessible
    if (node.package_declaration) {
        current_package = findPackageDeclaration(*node.package_declaration);
    }

    // Find current type (i.e the type that the current file specifies)
    if (current_type_name != "") {
        if (auto result = tryFindTypeInPackage(current_type_name, current_package)) {
            current_type = *result;
        } 
    }

    // Make import-on-demand packages accessible
    for (auto &qualified_identifier : node.type_import_on_demand_declaration) {
        star_imports.emplace_back(findPackageDeclaration(qualified_identifier));
    }

    // Make imported types accessible
    for (auto &qualified_identifier : node.single_type_import_declaration) {
        TypeDeclaration imported_type = findTypeImport(qualified_identifier);

        auto package_qualifier = qualified_identifier.getQualifiedIdentifierWithoutLast();
        PackageDeclarationObject* imported_types_package = findPackageDeclaration(package_qualifier);

        // Add single import if we didn't import a package with that type; ignore otherwise
        if (std::find(star_imports.begin(), star_imports.end(), imported_types_package) == star_imports.end()) {
            single_imports.emplace_back(imported_type);
        }
    }

    // Ignore duplicate imports
    util::removeDuplicates(single_imports);
    util::removeDuplicates(star_imports);

    // Check that no canonical name of two types overlaps
    for (auto imported_type1 : single_imports) {
        auto it = std::find_if(single_imports.begin(), single_imports.end(), [&](auto imported_type2){
            bool names_match = (getIdentifier(imported_type1) == getIdentifier(imported_type2));
            bool different_types = (imported_type1 != imported_type2);
            return names_match && different_types;
        });
        if (it != single_imports.end()) {
            throw SemanticError(
                "Canonical name of imported type " + getIdentifier(imported_type1) + " ambiguous.");
        }
    }

    this->visit_children(node);
}

void TypeLinker::operator()(ClassInstanceCreationExpression &node) {
    this->visit_children(node);
    node.linked_class_type = lookupQualifiedType(*node.class_name);
}

void TypeLinker::operator()(Type &node) {
    if(node.non_array_type == nullptr) {
        throw CompilerDevelopmentError("Non array type is null");
    }
    
    // Resolve type if it refers to an identifier
    if (auto qualified_identifier = std::get_if<QualifiedIdentifier>(node.non_array_type.get())) {
        node.link = lookupQualifiedType(*qualified_identifier);
    }

    this->visit_children(node);
}

void TypeLinker::operator()(ClassDeclaration &node) {

    std::string name = node.class_name->name;

    // Check this class resolves unambiguously with imported classes
    auto qid = QualifiedIdentifier(std::vector{Identifier(name)});
    lookupQualifiedType(qid);

    // Resolve implemented types to interfaces
    for (auto &implements_qualified_identifier : node.implements) {
        TypeDeclaration implemented = lookupQualifiedType(implements_qualified_identifier);
        if (auto interface_type = std::get_if<InterfaceDeclarationObject*>(&implemented)) {
            node.environment->implemented.emplace_back(*interface_type);
        } else {
            throw SemanticError("Class attempting to implement class");
        }
    }

    // Resolve extended type to class
    if (node.extends_class) {
        TypeDeclaration extended = lookupQualifiedType(*node.extends_class);
        if (auto class_type = std::get_if<ClassDeclarationObject*>(&extended)) {
            node.environment->extended = *class_type;
        } else {
            throw SemanticError("Class attempting to extend interface");
        }
    }

    visit_children(node);
}

void TypeLinker::operator()(InterfaceDeclaration &node) {

    std::string name = node.interface_name->name;

    // Check this interface resolves unambiguously with imported classes
    auto qid = QualifiedIdentifier(std::vector{Identifier(name)});
    lookupQualifiedType(qid);

    // Resolve extended types to interfaces
    for (auto &extends_qualified_identifier : node.extends_class) {
        TypeDeclaration extended = lookupQualifiedType(extends_qualified_identifier);
        if (auto interface_type = std::get_if<InterfaceDeclarationObject*>(&extended)) {
            node.environment->extended.emplace_back(*interface_type);
        } else {
            throw SemanticError("Interface attempting to extend class");
        }
    }

    visit_children(node);
}

void TypeLinker::operator()(FieldDeclaration &node) {
    // Resolve type
    visit_children(node);
    node.environment->type = node.type->link;
}

void TypeLinker::operator()(MethodDeclaration &node) {
    // Resolve return type
    visit_children(node);
    if (node.type) {
        // This method is not a constructor
        node.environment->return_type = node.type->link;
    } else {
        node.environment->is_constructor = true;
    }
}

void TypeLinker::operator()(FormalParameter &node) {
    // Resolve type
    visit_children(node);
    node.environment->type = node.type->link;
}

void TypeLinker::operator()(LocalVariableDeclaration &node) {
    // Resolve type
    visit_children(node);
    node.environment->type = node.type->link;
}
