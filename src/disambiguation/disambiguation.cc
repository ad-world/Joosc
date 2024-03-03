#include "disambiguation.h"
#include "type-decl/type_declaration.h"
#include "variant-ast/names.h"
#include "environment-builder/symboltable.h"
#include "exceptions/exceptions.h"

void DisambiguationVisitor::operator()(MethodInvocation &node) {
    auto &method_name = node.method_name;

    if(std::holds_alternative<QualifiedIdentifier>(*method_name)) {
        auto &qi = std::get<QualifiedIdentifier>(*method_name);
        disambiguate(qi);
    }
}

void DisambiguationVisitor::operator()(ClassInstanceCreationExpression &node) {
    auto &class_name = node.class_name;
    disambiguate(*class_name);
}

void DisambiguationVisitor::operator()(FieldAccess &node) {
    auto &expression = node.expression;

    // For field access, we need to disambiguate the expression, and then check if the identifier is a field / method of the class
    if(std::holds_alternative<QualifiedIdentifier>(*expression)) {
        auto &qi = std::get<QualifiedIdentifier>(*expression);
        disambiguate(qi);
    }

    // Get the linked type of the field access expression
    auto linked_type = getLink(expression);

    // if the linked type is a class, check if the identifier is a field of the class
    if(std::holds_alternative<struct ClassDeclarationObject*>(linked_type.linked_type)) {
        auto class_decl = std::get<struct ClassDeclarationObject*>(linked_type.linked_type);
        auto &identifier = node.identifier;

        if(class_decl->fields->lookupSymbol(identifier->name)) {
            identifier->classification = Classification::EXPRESSION_NAME;
        } else {
            THROW_DisambiguationError("Ambiguous field name " + node.identifier->name + " does not exist in type " + class_decl->identifier);
        }
    // if the linked type is an interface, check if the identifier is a method of the interface
    } else if (std::holds_alternative<struct InterfaceDeclarationObject*>(linked_type.linked_type)) {
        auto interface_decl = std::get<struct InterfaceDeclarationObject*>(linked_type.linked_type);
        auto &identifier = node.identifier;

        if(interface_decl->methods->lookupSymbol(identifier->name)) {
            identifier->classification = Classification::EXPRESSION_NAME;
        } else {
            THROW_DisambiguationError("Ambiguous method name " + node.identifier->name + " does not exist in type " + interface_decl->identifier);
        }
    } else {
        THROW_DisambiguationError("Ambiguous field access expression");
    }

}

void DisambiguationVisitor::operator()(ArrayAccess &node) {
    auto &array_ref_expression = node.array;

    if(std::holds_alternative<QualifiedIdentifier>(*array_ref_expression)) {
        auto &qi = std::get<QualifiedIdentifier>(*array_ref_expression);
        disambiguate(qi);
    }
}

void DisambiguationVisitor::operator()(Assignment &node) {
    auto &lhs = node.assigned_to;

    if (std::holds_alternative<QualifiedIdentifier>(*lhs)) {
        auto &lhs_qi = std::get<QualifiedIdentifier>(*lhs);
        disambiguate(lhs_qi);
    }
}


void DisambiguationVisitor::operator()(MethodDeclaration &node) {
    current_method = node.environment;
    this->visit_children(node);
    current_method = nullptr;
}

void DisambiguationVisitor::operator()(ClassDeclaration &node) {
    current_class = node.environment;
    this->visit_children(node);
    current_class = nullptr;
}

void DisambiguationVisitor::operator()(InterfaceDeclaration &node) {
    current_interface = node.environment;
    this->visit_children(node);
    current_interface = nullptr;
}

void DisambiguationVisitor::operator()(CompilationUnit &node) {
    compilation_unit = &node;
    this->visit_children(node);
    compilation_unit = nullptr;
}

void DisambiguationVisitor::disambiguate(QualifiedIdentifier &qi) {
    // if qi is a simple name, then we can disambiguate it
    if (qi.identifiers.size() == 1) {
        auto &identifier = qi.identifiers[0].name;

        // If the Identifier appears within the scope (§6.3) of a local variable declaration (§14.4) or parameter declaration (§8.4.1, §8.8.1, §14.19) or field declaration (§8.3) with that name, then the AmbiguousName is reclassified as an ExpressionName.
        if (current_method != nullptr) {
            // Check local variable declarations
            if (current_method->scope_manager.lookupVariable(identifier)) {
                qi.identifiers[0].classification = Classification::EXPRESSION_NAME;
                return;
            }

            // Check method parameters
            if (current_method->parameters->lookupSymbol(identifier)) {
                qi.identifiers[0].classification = Classification::EXPRESSION_NAME;
                return;
            }
        }

        if (current_class != nullptr) {
            // Check class fields
            if(current_class->fields->lookupSymbol(identifier)) {
                qi.identifiers[0].classification = Classification::EXPRESSION_NAME;
                return;
            }

            // Check local compilation unit for class
            if(current_class->ast_reference->class_name->name == identifier) {
                qi.identifiers[0].classification = Classification::TYPE_NAME;
                return;
            }
        }

        if (current_interface != nullptr) {
            // Check local compilation unit for interface
            if(current_interface->ast_reference->interface_name->name == identifier) {
                qi.identifiers[0].classification = Classification::TYPE_NAME;
                return;
            }
        }

        // Check single-type-import declaration
        for (auto &import : compilation_unit->single_type_import_declaration) {
            if (import.identifiers.back().name == identifier) {
                qi.identifiers[0].classification = Classification::TYPE_NAME;
                return;
            }
        }

        // Check current package
        auto &package_name = compilation_unit->package_declaration;

        // Check if the class is in the current package
        if (current_package.findClassDeclaration(package_name->identifiers)) {
            qi.identifiers[0].classification = Classification::TYPE_NAME;
            return;
        }

        // Check if the interface is in the current package
        if (current_package.findInterfaceDeclaration(package_name->identifiers)) {
            qi.identifiers[0].classification = Classification::TYPE_NAME;
            return;
        }

        // Check type-import-on-demand declaration, can only have one
        bool found = false;

        for (auto &import : compilation_unit->type_import_on_demand_declaration) {
            auto package_decl = default_package.findPackageDeclaration(import.identifiers);

            if (package_decl->classes->lookupSymbol(identifier) || package_decl->interfaces->lookupSymbol(identifier)) {
                if ( !found ) {
                    found = true;
                } else {
                    THROW_DisambiguationError("Ambiguous type name " + identifier);
                }
            }
        }

        // Return type name if one type was found
        if ( found ) {
            qi.identifiers[0].classification = Classification::TYPE_NAME;
            return;
        } 

        // Return package name as default
        qi.identifiers[0].classification = Classification::PACKAGE_NAME;
        return;
    } else {
        // if qi is a qualified name, then we need to disambiguate the prefix
        auto prefix = qi.getQualifiedIdentifierWithoutLast();
        auto cur_ident = qi.identifiers.back();

        // Get classification of the prefix
        disambiguate(prefix);
        auto cls = prefix.identifiers.back().classification;

        // based on the classification of the prefix, we can disambiguate the current identifier
        switch (cls) {
            case Classification::EXPRESSION_NAME:
                // Return expression name
                qi.identifiers.back().classification = Classification::EXPRESSION_NAME;
                break;
            case Classification::TYPE_NAME:
                // Check if the current identifier is a field or method of the class
                auto class_decl = default_package.findClassDeclaration(prefix.identifiers);
                if (class_decl->fields->lookupSymbol(cur_ident.name) || class_decl->methods->lookupSymbol(cur_ident.name)) {
                    qi.identifiers.back().classification = Classification::EXPRESSION_NAME;
                    break;
                }
                // Check if the current identifier is a method of the interface
                auto interface_decl = default_package.findInterfaceDeclaration(prefix.identifiers);
                if (interface_decl->methods->lookupSymbol(cur_ident.name)) {
                    qi.identifiers.back().classification = Classification::EXPRESSION_NAME;
                    break;
                }

                // If not, throw errr
                THROW_DisambiguationError("Ambiguous type name " + qi.getQualifiedName());
            case Classification::PACKAGE_NAME:
                // Get package object of prefix
                auto package_decl = default_package.findPackageDeclaration(prefix.identifiers);

                // Check if the current identifier is a class or interface in the package
                if (package_decl->classes->lookupSymbol(cur_ident.name) || package_decl->interfaces->lookupSymbol(cur_ident.name)) {
                    qi.identifiers.back().classification = Classification::TYPE_NAME;
                    break;
                }
                // If not, return package name
                qi.identifiers.back().classification = Classification::PACKAGE_NAME;
                break;
            default:
                THROW_DisambiguationError("Ambiguous type name " + qi.getQualifiedName());
                break;
        }
    }
}