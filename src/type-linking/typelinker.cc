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
#include "exceptions/exceptions.h"

using namespace std;

std::string getIdentifier(TypeDeclaration type_dec) {
    return std::visit([&](auto type) { return type->identifier; }, type_dec);
}

TypeLinker::TypeLinker(
    PackageDeclarationObject &default_package
) : 
    default_package{&default_package}
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
                THROW_TypeLinkerError("Package prefix resolves to type " + type_name);
            }
        }

        // Resolve to subpackage
        auto possible_package = temp_package->sub_packages->lookupUniqueSymbol(identifier.name);
        if (possible_package) {
            temp_package = &(std::get<PackageDeclarationObject>(*possible_package));
        } else {
            // Reference to undeclared package
            THROW_TypeLinkerError("Undeclared package: " + identifier.name + " in package " + temp_package->identifier);
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
            if (&identifier != &qualified_identifier.identifiers.back() && temp_package != default_package) {
                // This is only problematic if the possible class / interface is NOT in the default package
                THROW_TypeLinkerError("Prefix of fully qualified type resolves to type");
            }

            if (&identifier == &qualified_identifier.identifiers.back()) {
                if (possible_class) {
                    return util::getAsTypeDeclaration(possible_class);
                } else if (possible_interface) {
                    return util::getAsTypeDeclaration(possible_interface);
                }
                throw CompilerDevelopmentError("was not class or interface");
            }   
        }

        // Resolve to subpackage
        auto possible_package = temp_package->sub_packages->lookupUniqueSymbol(identifier.name);
        if (possible_package) {
            temp_package = &(std::get<PackageDeclarationObject>(*possible_package));
        } else {
            // Reference to undeclared package
            THROW_TypeLinkerError("Undeclared package");
        }
    }
    THROW_TypeLinkerError("Undeclared type imported");
}

/* Visitor implementation */

void TypeLinker::operator()(CompilationUnit &node) {
    PackageDeclarationObject *current_package;
    TypeDeclaration declared_type;
    std::vector<TypeDeclaration> single_imports;
    std::vector<PackageDeclarationObject*> star_imports;

    // Import java.lang implicitly
    auto java_lang = QualifiedIdentifier(std::vector<Identifier>{Identifier("java"), Identifier("lang")});
    star_imports.emplace_back(findPackageDeclaration(java_lang));

     // Make package of compilation unit accessible
    if (node.package_declaration) {
        current_package = findPackageDeclaration(*node.package_declaration);
    } else {
        current_package = this->default_package;
    }

    // Find current type (i.e the type that the current file specifies)
    std::string current_type_name;
    if (node.class_declarations.size() > 0) {
        current_type_name = node.class_declarations[0].class_name->name;
    } else if (node.interface_declarations.size() > 0) {
        current_type_name = node.interface_declarations[0].interface_name->name;
    }
    if (current_type_name != "") {
        if (auto result = current_package->tryFindTypeInPackage(current_type_name)) {
            declared_type = *result;
        } 
    }

    // Make import-on-demand packages accessible
    for (auto &qualified_identifier : node.type_import_on_demand_declaration) {
        star_imports.emplace_back(findPackageDeclaration(qualified_identifier));
    }

    // Make imported single types accessible
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
            THROW_TypeLinkerError(
                "Canonical name of imported type " + getIdentifier(imported_type1) + " ambiguous.");
        }
    }

    // Build namespace
    node.cu_namespace = CompilationUnitNamespace(
        current_package,
        this->default_package,
        declared_type,
        single_imports,
        star_imports
    );
    this->compilation_unit_namespace = node.cu_namespace;

    this->visit_children(node);
}

void TypeLinker::operator()(ClassInstanceCreationExpression &node) {
    this->visit_children(node);
    node.linked_class_type = compilation_unit_namespace.lookupQualifiedType(*node.class_name);
}

void TypeLinker::operator()(Type &node) {
    if(node.non_array_type == nullptr) {
        throw CompilerDevelopmentError("Non array type is null");
    }
    
    // Resolve type if it refers to an identifier
    if (auto qualified_identifier = std::get_if<QualifiedIdentifier>(node.non_array_type.get())) {
        node.link = LinkedType(
            compilation_unit_namespace.lookupQualifiedType(*qualified_identifier),
            node.is_array
        );
    } else {
        // Type linked to primitive type
        node.link = LinkedType(
            std::get<PrimitiveType>(*node.non_array_type),
            node.is_array
        );
    }

    this->visit_children(node);
}

void TypeLinker::operator()(ClassDeclaration &node) {

    std::string name = node.class_name->name;

    // Check this class resolves unambiguously with imported classes
    auto qid = QualifiedIdentifier(std::vector{Identifier(name)});
    compilation_unit_namespace.lookupQualifiedType(qid);

    // Resolve implemented types to interfaces
    for (auto &implements_qualified_identifier : node.implements) {
        TypeDeclaration implemented = compilation_unit_namespace.lookupQualifiedType(implements_qualified_identifier);
        if (auto interface_type = std::get_if<InterfaceDeclarationObject*>(&implemented)) {
            node.environment->implemented.emplace_back(*interface_type);
        } else {
            THROW_TypeLinkerError("Class attempting to implement class");
        }
    }

    // Resolve extended type to class
    if (node.extends_class) {
        TypeDeclaration extended = compilation_unit_namespace.lookupQualifiedType(*node.extends_class);
        if (auto class_type = std::get_if<ClassDeclarationObject*>(&extended)) {
            node.environment->extended = *class_type;
        } else {
            THROW_TypeLinkerError("Class attempting to extend interface");
        }
    }

    visit_children(node);
}

void TypeLinker::operator()(InterfaceDeclaration &node) {

    std::string name = node.interface_name->name;

    // Check this interface resolves unambiguously with imported classes
    auto qid = QualifiedIdentifier(std::vector{Identifier(name)});
    compilation_unit_namespace.lookupQualifiedType(qid);

    // Resolve extended types to interfaces
    for (auto &extends_qualified_identifier : node.extends_class) {
        TypeDeclaration extended = compilation_unit_namespace.lookupQualifiedType(extends_qualified_identifier);
        if (auto interface_type = std::get_if<InterfaceDeclarationObject*>(&extended)) {
            node.environment->extended.emplace_back(*interface_type);
        } else {
            THROW_TypeLinkerError("Interface attempting to extend class");
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
