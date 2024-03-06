#include "forward-decl.h"
#include "exceptions/exceptions.h"
#include "environment-builder/scope.h"
#include "environment-builder/symboltableentry.h"
#include "environment-builder/symboltable.h"

void ForwardDeclarationVisitor::operator()(MethodDeclaration &node) {
    current_method = node.environment;
    this->visit_children(node);
    current_method = nullptr;
}

void ForwardDeclarationVisitor::operator()(ClassDeclaration &node) {
    current_class = node.environment;
    this->visit_children(node);
    current_class = nullptr;
}

void ForwardDeclarationVisitor::operator()(InterfaceDeclaration &node) {
    current_interface = node.environment;
    this->visit_children(node);
    current_interface = nullptr;
}

void ForwardDeclarationVisitor::operator()(CompilationUnit &node) {
    compilation_unit = &node;
    this->visit_children(node);
    compilation_unit = nullptr;
}

void ForwardDeclarationVisitor::operator()(FieldDeclaration &node) {
    current_identifier = node.variable_declarator->variable_name->name;
    this->visit_children(node);
    current_identifier = "";
}

void ForwardDeclarationVisitor::operator()(Assignment &node) {
    if(!std::holds_alternative<QualifiedIdentifier>(*node.assigned_to)) {
        this->visit_children(*node.assigned_to);
    }
    this->visit_children(*node.assigned_from);
}

void ForwardDeclarationVisitor::operator()(LocalVariableDeclaration &node) {
    current_local = node.variable_declarator->variable_name->name;
    this->visit_children(node);
    current_local = "";
}

void ForwardDeclarationVisitor::operator()(Block &node) {
    size_t scope_id = node.scope_id;

    if (current_method != nullptr) {
        current_method->scope_manager.openScope(scope_id);
    }

    this->visit_children(node);

    if (current_method != nullptr) {
        current_method->scope_manager.closeScope(scope_id);
    }
}


void ForwardDeclarationVisitor::operator()(QualifiedIdentifier &node) {
    auto current_ns = compilation_unit->cu_namespace;
    if (current_method != nullptr && current_local != "") {
        auto offender = node.getQualifiedName();
        if (!current_class->fields->lookupSymbol(offender)) {
            if (current_method->scope_manager.lookupVariable(current_local) && current_method->scope_manager.lookupVariable(offender)) {
                int current_decl_idx = current_method->scope_manager.getInsertPosition(current_local);
                int forward_decl_idx = current_method->scope_manager.getInsertPosition(offender);
                if (current_decl_idx < forward_decl_idx) {
                    THROW_DisambiguationError("Field declaration of " + current_local + " uses forward declaration of " + offender);
                }
            }
        }
    } else if (current_class != nullptr && current_identifier != "") {
        if (node.identifiers.front().name == current_identifier) {
            try {
                auto type = current_ns.lookupQualifiedType(node);
                if (std::holds_alternative<ClassDeclarationObject*>(type)) {
                    auto item = std::get<ClassDeclarationObject*>(type);
                    if (item != nullptr) return;
                }  else if (std::holds_alternative<InterfaceDeclarationObject*>(type)) {
                    auto item = std::get<InterfaceDeclarationObject*>(type);
                    if (item != nullptr) return;
                }
            } catch (...) {
                // do nothing, this just means that type was not found
            }

            throw DisambiguationError("Field declaration of " + current_identifier + " refers to itself.");
        };

        if (current_class->fields->lookupSymbol(current_identifier) && current_class->fields->lookupSymbol(node.identifiers.front().name)) {
            int current_decl_idx = current_class->fields->getInsertPosition(current_identifier);
            int forward_decl_idx = current_class->fields->getInsertPosition(node.identifiers.front().name);

            if (current_decl_idx < forward_decl_idx) {
                THROW_DisambiguationError("Field declaration of " + current_identifier + " uses forward declaration of " + node.identifiers.front().name);
            }
        }
    }

    this->visit_children(node);
}



void ForwardDeclarationVisitor::operator()(MethodInvocation &node) {
    this->visit_children(node);
}




