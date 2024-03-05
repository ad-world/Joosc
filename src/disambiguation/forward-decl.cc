#include "forward-decl.h"
#include "exceptions/exceptions.h"

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
    this->visit_children(*node.assigned_from);
}

void ForwardDeclarationVisitor::operator()(QualifiedIdentifier &node) {
    if(current_class && current_identifier != "") {
        if (node.identifiers.front().name == current_identifier) {
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
}


