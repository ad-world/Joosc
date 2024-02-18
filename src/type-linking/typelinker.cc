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

PackageDeclarationObject* TypeLinker::resolveToPackage(
    QualifiedIdentifier &qualified_identifier, 
    PackageDeclarationObject* source_package
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
            throw SemanticError("Undeclared package");
        }
    }

    return temp_package;
}

TypeDeclaration TypeLinker::resolveToType(QualifiedIdentifier &qualified_identifier) {
    PackageDeclarationObject* temp_package = default_package;

    for (auto &identifier : qualified_identifier.identifiers) {
        // Verify the qualified identifier resolves to a type at the last identifier, and the last only
        auto possible_class = temp_package->classes->lookupUniqueSymbol(identifier.name);
        auto possible_interface = temp_package->interfaces->lookupUniqueSymbol(identifier.name);

        if (possible_class || possible_interface) {
            if (&identifier != &qualified_identifier.identifiers.back()) {
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

TypeDeclaration TypeLinker::lookupType(QualifiedIdentifier &qualified_identifier) {
    std::string canoncial_name = qualified_identifier.identifiers.back().name;

    if (qualified_identifier.identifiers.size() == 1) {
        return lookupToSimpleType(canoncial_name);
    }

    std::vector<TypeDeclaration> valid_candidates;
    QualifiedIdentifier package_qid = qualified_identifier.getQualifiedIdentifierWithoutLast();

    // Check each package in compilation units namespace
    for (auto package : star_imports) {
        auto candidate_package = resolveToPackage(package_qid, package);
        if (auto possible_type = tryFindTypeInPackage(canoncial_name, candidate_package)) {
            valid_candidates.push_back(*possible_type);
        }
    }

    return resolveCandidates(valid_candidates, canoncial_name);
}

TypeDeclaration TypeLinker::lookupToSimpleType(std::string &identifier) {
    std::vector<TypeDeclaration> valid_candidates;

    // Look up in list of all single type imports
    for (auto type_dec : single_imports) {
        std::visit([&](auto class_or_int_dec) {
            if (class_or_int_dec->identifier == identifier) {
                valid_candidates.push_back(type_dec);
            }
        }, type_dec);
    }

    // Look up in imported packages
    for (auto package_dec : star_imports) {
        auto possible_classes = package_dec->classes->lookupSymbol(identifier);
        auto possible_interfaces = package_dec->interfaces->lookupSymbol(identifier);

        if (possible_classes) {
            TypeDeclaration found_type = &std::get<ClassDeclarationObject>(possible_classes->back());
            valid_candidates.push_back(found_type);
        } else if (possible_interfaces) {
            TypeDeclaration found_type = &std::get<InterfaceDeclarationObject>(possible_interfaces->back());
            valid_candidates.push_back(found_type);
        }
    }

    return resolveCandidates(valid_candidates, identifier);
}

/* Visitor implementation */

void TypeLinker::operator()(CompilationUnit &node) {
    // Import java.lang implicitly
    auto java_lang = QualifiedIdentifier(std::vector<Identifier>{Identifier("java"), Identifier("lang")});
    star_imports.emplace_back(resolveToPackage(java_lang, default_package));

    // Make import-on-demand packages accessible
    for (auto &qualified_identifier : node.type_import_on_demand_declaration) {
        star_imports.emplace_back(resolveToPackage(qualified_identifier, default_package));
    }

    // Make package of compilation unit accessible
    if (node.package_declaration) {
        current_package = resolveToPackage(*node.package_declaration, default_package);
    }
    star_imports.emplace_back(current_package);

    // Make imported types accessible
    for (auto &qualified_identifier : node.single_type_import_declaration) {
        TypeDeclaration imported_type = resolveToType(qualified_identifier);

        auto package_qualifier = qualified_identifier.getQualifiedIdentifierWithoutLast();
        PackageDeclarationObject* imported_types_package = resolveToPackage(package_qualifier, default_package);

        // Add single import if we didn't import a package with that type; ignore otherwise
        if (std::find(star_imports.begin(), star_imports.end(), imported_types_package) == star_imports.end()) {
            single_imports.emplace_back(imported_type);
        }
    }

    // Ignore duplicate imports
    util::removeDuplicates(single_imports);
    util::removeDuplicates(star_imports);

    // Check that no canonincal name of two types overlaps
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
}

void TypeLinker::operator()(Type &node) {
    if(node.non_array_type == nullptr) {
        throw CompilerDevelopmentError("Non array type is null");
    }
    
    // Resolve type if it refers to an identifier
    if (auto qualified_identifier = std::get_if<QualifiedIdentifier>(node.non_array_type.get())) {
        node.node = lookupType(*qualified_identifier);
    }

    this->visit_children(node);
}

void TypeLinker::operator()(ClassDeclaration &node) {

    std::string name = node.class_name->name;

    // Resolve implemented types to interfaces
    for (auto &implements_qualified_identifier : node.implements) {
        TypeDeclaration implemented = lookupType(implements_qualified_identifier);
        if (auto interface_type = std::get_if<InterfaceDeclarationObject*>(&implemented)) {
            node.environment->implemented.emplace_back(*interface_type);
        } else {
            throw SemanticError("Class attempting to implement class");
        }
    }

    // Resolve extended type to class
    if (node.extends_class) {
        TypeDeclaration extended = lookupType(*node.extends_class);
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

    // Resolve extended types to interfaces
    for (auto &extends_qualified_identifier : node.extends_class) {
        TypeDeclaration extended = lookupType(extends_qualified_identifier);
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
    // node.environment->type = node.type->node;
}

void TypeLinker::operator()(MethodDeclaration &node) {
    // Resolve return type
    visit_children(node);
    // node.environment->return_type = node.type->node;
}

void TypeLinker::operator()(FormalParameter &node) {
    // Resolve type
    visit_children(node);
    // node.environment->type = node.type->node;
}

void TypeLinker::operator()(LocalVariableDeclaration &node) {
    // Resolve type
    visit_children(node);
    // node.environment->type = node.type->node;
}
