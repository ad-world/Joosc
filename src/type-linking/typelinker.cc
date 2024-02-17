#include "typelinker.h"
#include <unordered_map>
#include <set>
#include <iostream>
#include <environment-builder/symboltable.h>
#include <variant>
#include <unordered_map>
#include "exceptions/compilerdevelopmenterror.h"


using namespace std;

TypeLinker::TypeLinker(
    PackageDeclarationObject &env, 
    CompilationUnit &ast_root, 
    vector<AstNodeVariant> &asts
) :       
    root_env{env},
    asts{asts},
    ast_root{&ast_root},

    current_package{&root_env},
    default_package{&root_env}
{
    package_name = "";
    if (ast_root.package_declaration.get() != nullptr) {
        package_name = ast_root.package_declaration.get()->getQualifiedName();
    }
};

// Resolve qualified identifier to package, or throw semantic error if it resolves to a type
PackageDeclarationObject* TypeLinker::resolveToPackage(QualifiedIdentifier &qualified_identifier) {
    PackageDeclarationObject* temp_package = default_package;

    for (auto &identifier : qualified_identifier.identifiers) {
        // Verify doesn't resolve to class not in default package
        if (!temp_package->is_default_package) {
            auto possible_class = temp_package->classes->lookupUniqueSymbol(identifier.name);
            auto possible_interface = temp_package->interfaces->lookupUniqueSymbol(identifier.name);
            if (possible_class || possible_interface) {
                throw SemanticError("Package prefix resolves to type");
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

// Resolve qualified identifier to fully qualified type
TypeDeclaration TypeLinker::resolveToTypeInPackage(QualifiedIdentifier &qualified_identifier) {
    PackageDeclarationObject* temp_package = default_package;

    for (auto &identifier : qualified_identifier.identifiers) {
        // Verify the qualified identifier resolves to a type at the last identifier, and the last only
        auto possible_class = temp_package->classes->lookupUniqueSymbol(identifier.name);
        auto possible_interface = temp_package->interfaces->lookupUniqueSymbol(identifier.name);

        if (possible_class || possible_interface) {
            if (&identifier != &qualified_identifier.identifiers.back()) {
                throw SemanticError("Prefix of fully qualified type resolves to type");
            } else if (possible_class) {
                return &(std::get<ClassDeclarationObject>(*possible_class));
            } else if (possible_interface) {
                return &(std::get<InterfaceDeclarationObject>(*possible_interface));
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
    throw SemanticError("Undeclared type imported");
}

// Resolve qualified identifer to type, or throw semantic error if it resolves to a package
TypeDeclaration TypeLinker::resolveToType(QualifiedIdentifier &qualified_identifier) {
    if (qualified_identifier.identifiers.size() == 1) {
        return simpleResolveToType(qualified_identifier.identifiers[0].name);
    }
    return static_cast<ClassDeclarationObject*>(nullptr);
}


TypeDeclaration TypeLinker::simpleResolveToType(std::string &identifier) {
    auto possible_class = current_package->classes->lookupUniqueSymbol(identifier);
    auto possible_interface = current_package->interfaces->lookupUniqueSymbol(identifier);

    if (possible_class) {
        return &(std::get<ClassDeclarationObject>(*possible_class));
    } else if (possible_interface) {
        return &(std::get<InterfaceDeclarationObject>(*possible_interface));
    } else {
        throw CompilerDevelopmentError("Type declaration does not exist");
    }
}

void TypeLinker::checkTypeOverlap() {
    for (auto type_dec : single_imports) {
        std::string name;
        std::visit([&](auto type) { name = type->identifier; }, type_dec);

        if (simple_types_available.count(name) && simple_types_available[name] != type_dec) {
            throw SemanticError("Conflicting declarations of the same type accessible");
        }
        simple_types_available[name] = type_dec;
    }
}

void TypeLinker::operator()(CompilationUnit &node) {
    // Resolve package declaration
    if (node.package_declaration) {
        current_package = resolveToPackage(*node.package_declaration);
    }

    // Import java.lang implicitly
    auto java_lang = QualifiedIdentifier(std::vector<Identifier>{Identifier("java"), Identifier("lang")});
    star_imports.emplace_back(resolveToPackage(java_lang));

    for (auto &qualified_identifier : node.single_type_import_declaration) {
        // Resolve to single type import
        single_imports.emplace_back(resolveToTypeInPackage(qualified_identifier));
    }
    for (auto &qualified_identifier : node.type_import_on_demand_declaration) {
        // Resolve to package import
        star_imports.emplace_back(resolveToPackage(qualified_identifier));
    }

    checkTypeOverlap();

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
        node.node = resolveToType(*qualified_identifier);
    }

    this->visit_children(node);
}

void TypeLinker::operator()(ClassDeclaration &node) {

    std::string name = node.class_name->name;
    if (simple_types_available.count(name) && 
            simple_types_available[name] != static_cast<TypeDeclaration>(node.environment)) {
        throw SemanticError("Conflicting declarations of the same type accessible");
    }

    // Resolve implemented types to interfaces
    for (auto &implements_qualified_identifier : node.implements) {
        TypeDeclaration implemented = resolveToType(implements_qualified_identifier);
        if (auto interface_type = std::get_if<InterfaceDeclarationObject*>(&implemented)) {
            node.environment->implemented.emplace_back(*interface_type);
        } else {
            throw SemanticError("Class attempting to implement class");
        }
    }

    // Resolve extended type to class
    if (node.extends_class) {
        TypeDeclaration extended = resolveToType(*node.extends_class);
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
    if (simple_types_available.count(name) && 
            simple_types_available[name] != static_cast<TypeDeclaration>(node.environment)) {
        throw SemanticError("Conflicting declarations of the same type accessible");
    }

    // Resolve extended types to interfaces
    for (auto &extends_qualified_identifier : node.extends_class) {
        TypeDeclaration extended = resolveToType(extends_qualified_identifier);
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
