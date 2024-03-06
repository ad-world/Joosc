#include "disambiguation.h"
#include "type-decl/type_declaration.h"
#include "variant-ast/classes.h"
#include "variant-ast/names.h"
#include "variant-ast/astvisitor/graballvisitor.h"
#include "environment-builder/symboltable.h"
#include "exceptions/exceptions.h"
#include "type-linking/compilation_unit_namespace.h"
#include <iostream>
#include <variant>
#include <algorithm>

void DisambiguationVisitor::operator()(MethodInvocation &node) {
    auto &parent_expr = node.parent_expr;
    node.method_name->classification = Classification::METHOD_NAME;
    if( parent_expr && std::holds_alternative<QualifiedIdentifier>(*parent_expr) ) {
        auto &qi = std::get<QualifiedIdentifier>(*parent_expr);
        disambiguate(qi, qi.identifiers.size() - 1);
    }
    this->visit_children(node);
}

void DisambiguationVisitor::operator()(ClassInstanceCreationExpression &node) {
    auto &class_name = node.class_name;
    disambiguate(*class_name, class_name->identifiers.size() - 1);
    this->visit_children(node);
}

void DisambiguationVisitor::operator()(FieldAccess &node) {
    auto &expression = node.expression;

    node.identifier->classification = Classification::EXPRESSION_NAME;
    // For field access, we need to disambiguate the expression, and then check if the identifier is a field / method of the class
    if(std::holds_alternative<QualifiedIdentifier>(*expression)) {
        auto &qi = std::get<QualifiedIdentifier>(*expression);
        disambiguate(qi, qi.identifiers.size() - 1);
    }

    this->visit_children(node);
}

void DisambiguationVisitor::operator()(CastExpression &node) {
    if(std::holds_alternative<QualifiedIdentifier>(*node.expression)) {
        auto expr = std::get<QualifiedIdentifier>(*node.expression);
    }
    this->visit_children(node);
}

void DisambiguationVisitor::operator()(ParenthesizedExpression &node) {
    if(std::holds_alternative<QualifiedIdentifier>(*node.expression)) {
        auto expr = std::get<QualifiedIdentifier>(*node.expression);
        disambiguate(expr, expr.identifiers.size() - 1);

        if (expr.identifiers.back().classification == Classification::TYPE_NAME) {
            THROW_DisambiguationError("Parenthesized expression resolves to Type, not expression");
        }
    }
    this->visit_children(node);
}

void DisambiguationVisitor::operator()(ArrayAccess &node) {
    auto &array_ref_expression = node.array;

    if(std::holds_alternative<QualifiedIdentifier>(*array_ref_expression)) {
        auto &qi = std::get<QualifiedIdentifier>(*array_ref_expression);
        disambiguate(qi, qi.identifiers.size() - 1);
    }
    this->visit_children(node);
}

void DisambiguationVisitor::operator()(Assignment &node) {
    auto &lhs = node.assigned_to;

    if (std::holds_alternative<QualifiedIdentifier>(*lhs)) {
        auto &lhs_qi = std::get<QualifiedIdentifier>(*lhs);
        disambiguate(lhs_qi, lhs_qi.identifiers.size() - 1);
    }

    this->visit_children(node);
}


void DisambiguationVisitor::operator()(MethodDeclaration &node) {
    current_method = node.environment;
    node.function_name->classification = Classification::METHOD_NAME;
    this->visit_children(node);
    current_method = nullptr;
}

void DisambiguationVisitor::operator()(ClassDeclaration &node) {
    current_class = node.environment;
    node.class_name->classification = Classification::TYPE_NAME;
    this->visit_children(node);
    current_class = nullptr;
}

void DisambiguationVisitor::operator()(InterfaceDeclaration &node) {
    current_interface = node.environment;
    node.interface_name->classification = Classification::TYPE_NAME;
    this->visit_children(node);
    current_interface = nullptr;
}

void DisambiguationVisitor::operator()(CompilationUnit &node) {
    compilation_unit = &node;
    
    for (auto &import : node.single_type_import_declaration) {
        for (auto& ident: import.identifiers) {
            if (&ident == &import.identifiers.back()) {
                ident.classification = Classification::TYPE_NAME;
            } else {
                ident.classification = Classification::PACKAGE_NAME;
            }
        }
    }

    for (auto &import: node.type_import_on_demand_declaration) {
        for (auto& ident: import.identifiers) {
            ident.classification = Classification::PACKAGE_NAME;
        }
    }

    if(node.package_declaration) {
        for (auto& ident: node.package_declaration->identifiers) {
            ident.classification = Classification::PACKAGE_NAME;
        }

        current_package = default_package->findPackageDeclaration(node.package_declaration->identifiers);
    }

    this->visit_children(node);
    compilation_unit = nullptr;
    current_package = nullptr;
}

void DisambiguationVisitor::operator()(QualifiedIdentifier &node) {
    if (node.identifiers.back().classification == Classification::UNCLASSIFIED) {
        disambiguate(node, node.identifiers.size() - 1);
    }
    this->visit_children(node);
}

void DisambiguationVisitor::operator()(Block &node) {
    size_t scope_id = node.scope_id;

    if (current_method != nullptr) {
        current_method->scope_manager.openScope(scope_id);
    }

    this->visit_children(node);

    if (current_method != nullptr) {
        current_method->scope_manager.closeScope(scope_id);
    }
}

void DisambiguationVisitor::disambiguate(QualifiedIdentifier &ref, int current_pos) {
    // if qi is a simple name, then we can disambiguate it
    auto &current_ns = compilation_unit->cu_namespace;
    auto cur_ident = ref.identifiers[current_pos];

    if (current_pos < 0) return;
    if (current_pos == 0) {
        auto &identifier = ref.identifiers[current_pos].name;

        // If the Identifier appears within the scope (§6.3) of a local variable declaration (§14.4) or parameter declaration (§8.4.1, §8.8.1, §14.19) or field declaration (§8.3) with that name, then the AmbiguousName is reclassified as an ExpressionName.
        if (current_method != nullptr) {
            size_t scope_id = current_method->ast_reference->body->scope_id;

            if (current_method->scope_manager.lookupVariable(identifier)) {
                ref.identifiers[current_pos].classification = Classification::EXPRESSION_NAME;
                return;
            }
            // Check method parameters
            if (current_method->parameters->lookupSymbol(identifier)) {
                ref.identifiers[current_pos].classification = Classification::EXPRESSION_NAME;
                return;
            }
        }

        if (current_class != nullptr) {
            // Check class fields
            if(current_class->fields->lookupSymbol(identifier) || (current_class->accessible_fields.find(identifier) != current_class->accessible_fields.end())) {
                ref.identifiers[current_pos].classification = Classification::EXPRESSION_NAME;
                return;
            }

            // Check local compilation unit for class
            if(current_class->ast_reference->class_name->name == identifier) {
                ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
                return;
            }
        }

        if (current_interface != nullptr) {
            // Check local compilation unit for interface
            if(current_interface->ast_reference->interface_name->name == identifier) {
                ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
                return;
            }
        }        

        // Check single-type-import declaration
        for (auto &import : compilation_unit->single_type_import_declaration) {
            if (import.identifiers.back().name == identifier) {
                ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
                return;
            }
        }

        // Check current package
        auto &package_name = compilation_unit->package_declaration;

        if ( current_package && package_name ) {
            // Check if the class is in the current package
            if (current_package->findClassDeclaration(cur_ident.name)) {
                ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
                return;
            }

            // Check if the interface is in the current package
            if (current_package->findInterfaceDeclaration(cur_ident.name)) {
                ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
                return;
            }
        } else {
            if (default_package->findClassDeclaration(cur_ident.name)) {
                ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
                return;
            }

            // Check if the interface is in the current package
            if (default_package->findInterfaceDeclaration(cur_ident.name)) {
                ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
                return;
            }
        }


        // Check type-import-on-demand declaration, can only have one
        bool found = false;

        { // Checking java.lang
            auto package_decl = default_package->findPackageDeclaration("java.lang");

            if (package_decl->classes->lookupSymbol(identifier) || package_decl->interfaces->lookupSymbol(identifier)) {
                if ( !found ) {
                    found = true;
                } else {
                    THROW_DisambiguationError("Ambiguous type name " + identifier);
                }
            }
        }

        // Checking rest of imports
        for (auto &import : compilation_unit->type_import_on_demand_declaration) {
            auto package_decl = default_package->findPackageDeclaration(import.identifiers);

            if (package_decl->classes->lookupSymbol(identifier) || package_decl->interfaces->lookupSymbol(identifier)) {
                if ( !found ) {
                    found = true;
                } else {
                    if (import.getQualifiedName() != "java.lang") {
                        THROW_DisambiguationError("Ambiguous type name " + identifier);
                    }
                }
            }
        }

        // Return type name if one type was found
        if ( found ) {
            ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
            return;
        } 

        // Return package name as default
        if (default_package->sub_packages->lookupSymbol(ref.identifiers[current_pos].name)) {
            auto package = default_package->sub_packages->lookupSymbol(ref.identifiers[current_pos].name);
            ref.identifiers[current_pos].classification = Classification::PACKAGE_NAME;
        } else {
            THROW_DisambiguationError("Ambiguous type name " + ref.getQualifiedName());
        }
        return;
    } else {
        // if qi is a qualified name, then we need to disambiguate the prefix

        // Get classification of the prefix
        disambiguate(ref, current_pos - 1);
        auto cls = ref.identifiers[current_pos - 1].classification;
        std::vector<Identifier> copy;

        for(int i = 0; i < current_pos; i++) {
            copy.push_back(ref.identifiers[i]);
        }

        auto prefix = QualifiedIdentifier(copy);
    
        // based on the classification of the prefix, we can disambiguate the current identifier
        switch (cls) {
            case Classification::EXPRESSION_NAME: {
                ref.identifiers[current_pos].classification = Classification::EXPRESSION_NAME;
            } break;
                // Return expression name
            case Classification::TYPE_NAME: {
                // Check namspace
                auto type = current_ns.lookupQualifiedType(prefix);
                // Check if the current identifier is a field or method of the class
                if (std::holds_alternative<ClassDeclarationObject*>(type)) {
                    auto class_decl = std::get<ClassDeclarationObject*>(type);
                    
                    if (class_decl->fields->lookupUniqueSymbol(cur_ident.name) || class_decl->methods->lookupUniqueSymbol(cur_ident.name) || (class_decl->all_methods.find(cur_ident.name) != class_decl->all_methods.end())) {
                        ref.identifiers[current_pos].classification = Classification::EXPRESSION_NAME;
                        return;
                    }
                // Check if the current identifier is a method of the interface
                } else if (std::holds_alternative<InterfaceDeclarationObject*>(type)) {
                    auto interface_decl = std::get<InterfaceDeclarationObject*>(type);

                    if (interface_decl->methods->lookupUniqueSymbol(cur_ident.name)) {
                        ref.identifiers[current_pos].classification = Classification::EXPRESSION_NAME;
                        return;
                    }
                }

                THROW_DisambiguationError("Ambiguous type name " + ref.getQualifiedName());
            } break;
            case Classification::PACKAGE_NAME: {
                // Get package object of prefix
                auto package_decl = default_package->findPackageDeclaration(prefix.identifiers);

                // Check if the current identifier is a class or interface in the package
                if (package_decl && (package_decl->classes->lookupUniqueSymbol(cur_ident.name) || package_decl->interfaces->lookupUniqueSymbol(cur_ident.name))) {
                    ref.identifiers[current_pos].classification = Classification::TYPE_NAME;
                    return;
                }
                // If not, return package name
                if (package_decl && package_decl->sub_packages->lookupSymbol(cur_ident.name)) {
                    ref.identifiers[current_pos].classification = Classification::PACKAGE_NAME;
                    return;
                }

                THROW_DisambiguationError("Ambiguous type name " + ref.getQualifiedName());
            } break;
            default:
                THROW_DisambiguationError("Ambiguous type name " + ref.getQualifiedName());
        }
    }
}

void DisambiguationVisitor::checkForwardDeclaration(std::string usage, std::string potential_forward_dec) {
    // Check not the same reference
    if (potential_forward_dec == usage) {
        THROW_DisambiguationError("Field declaration of " + usage + " refers to itself.");
    }

    // Check not a forward declaration
    if (current_class->fields->lookupSymbol(usage) && current_class->fields->lookupSymbol(potential_forward_dec)) {
        int current_decl_idx = current_class->fields->getInsertPosition(usage);
        int forward_decl_idx = current_class->fields->getInsertPosition(potential_forward_dec);

        if (current_decl_idx < forward_decl_idx) {
            THROW_DisambiguationError("Field declaration of " + usage + " uses forward declaration of " + potential_forward_dec);
        }
    }
}

// Rest of unclassified names

void DisambiguationVisitor::operator()(VariableDeclarator &node) {
    node.variable_name->classification = Classification::TYPE_NAME;
    this->visit_children(node);
}

void DisambiguationVisitor::operator()(FormalParameter &node) {
    node.parameter_name->classification = Classification::TYPE_NAME;
    this->visit_children(node);
}
