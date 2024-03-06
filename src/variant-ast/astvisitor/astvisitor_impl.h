#pragma once

#include "astvisitor.h"

/* Packages */
template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(CompilationUnit &node) {
    if (node.package_declaration) {
        this->operator()(*node.package_declaration);
    }
    for (auto &child : node.single_type_import_declaration) {
        this->operator()(child);
    }
    for (auto &child : node.type_import_on_demand_declaration) {
        this->operator()(child);
    }
    for (auto &child : node.class_declarations) {
        this->operator()(child);
    }
    for (auto &child : node.interface_declarations) {
        this->operator()(child);
    }
}

/* Names */
template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(QualifiedIdentifier &node) {
    for (auto &child : node.identifiers) {
        this->operator()(child);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(Identifier &node) {}

/* Types */
template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(Type &node) {
    if (node.non_array_type) {
        this->operator()(*node.non_array_type);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(NonArrayType &node) {}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(PrimitiveType &node) {}

/* Classes */
template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(ClassDeclaration &node) {
    for (auto &child : node.modifiers) {
        this->operator()(child);
    }
    if (node.class_name) {
        this->operator()(*node.class_name);
    }
    if (node.extends_class) {
        this->operator()(*node.extends_class);
    }
    for (auto &child : node.implements) {
        this->operator()(child);
    }
    for (auto &child : node.field_declarations) {
        this->operator()(child);
    }
    for (auto &child : node.method_declarations) {
        this->operator()(child);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(InterfaceDeclaration &node) {
    for (auto &child : node.modifiers) {
        this->operator()(child);
    }
    if (node.interface_name) {
        this->operator()(*node.interface_name);
    }
    for (auto &child : node.extends_class) {
        this->operator()(child);
    }
    for (auto &child : node.method_declarations) {
        this->operator()(child);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(FieldDeclaration &node) {
    for (auto &child : node.modifiers) {
        this->operator()(child);
    }
    if (node.type) {
        this->operator()(*node.type);
    }
    if (node.variable_declarator) {
        this->operator()(*node.variable_declarator);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(MethodDeclaration &node) {
    for (auto &child : node.modifiers) {
        this->operator()(child);
    }
    if (node.type) {
        this->operator()(*node.type);
    }
    if (node.function_name) {
        this->operator()(*node.function_name);
    }
    for (auto &child : node.parameters) {
        this->operator()(child);
    }
    if (node.body) {
        this->operator()(*node.body);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(VariableDeclarator &node) {
    if (node.variable_name) {
        this->operator()(*node.variable_name);
    }
    if (node.expression) {
        this->operator()(*node.expression);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(FormalParameter &node) {
    if (node.type) {
        this->operator()(*node.type);
    }
    if (node.parameter_name) {
        this->operator()(*node.parameter_name);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(Modifier &node) {}

/* Statements */
template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(LocalVariableDeclaration &node) {
    if (node.type) {
        this->operator()(*node.type);
    }
    if (node.variable_declarator) {
        this->operator()(*node.variable_declarator);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(Block &node) {
    for (auto &child : node.statements) {
        this->operator()(child);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(IfThenStatement &node) {
    if (node.if_clause) {
        this->operator()(*node.if_clause);
    }
    if (node.then_clause) {
        this->operator()(*node.then_clause);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(IfThenElseStatement &node) {
    if (node.if_clause) {
        this->operator()(*node.if_clause);
    }
    if (node.then_clause) {
        this->operator()(*node.then_clause);
    }
    if (node.else_clause) {
        this->operator()(*node.else_clause);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(WhileStatement &node) {
    if (node.condition_expression) {
        this->operator()(*node.condition_expression);
    }
    if (node.body_statement) {
        this->operator()(*node.body_statement);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(ForStatement &node) {
    if (node.init_statement) {
        this->operator()(*node.init_statement);
    }
    if (node.condition_expression) {
        this->operator()(*node.condition_expression);
    }
    if (node.update_statement) {
        this->operator()(*node.update_statement);
    }
    if (node.body_statement) {
        this->operator()(*node.body_statement);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(ReturnStatement &node) {
    if (node.return_expression) {
        this->operator()(*node.return_expression);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(EmptyStatement &node) {}

/* Expressions */
template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(InfixExpression &node) {
    if (node.expression1) {
        this->operator()(*node.expression1);
    }
    if (node.expression2) {
        this->operator()(*node.expression2);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(PrefixExpression &node) {
    if (node.expression) {
        this->operator()(*node.expression);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(CastExpression &node) {
    if (node.type) {
        this->operator()(*node.type);
    }
    if (node.expression) {
        this->operator()(*node.expression);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(Assignment &node) {
    if (node.assigned_to) {
        this->operator()(*node.assigned_to);
    }
    if (node.assigned_from) {
        this->operator()(*node.assigned_from);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(QualifiedThis &node) {
    if (node.qualified_this) {
        this->operator()(*node.qualified_this);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(ArrayCreationExpression &node) {
    if (node.type) {
        this->operator()(*node.type);
    }
    if (node.expression) {
        this->operator()(*node.expression);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(Literal &node) {}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(ClassInstanceCreationExpression &node) {
    if (node.class_name) {
        this->operator()(*node.class_name);
    }
    for (auto &child : node.arguments) {
        this->operator()(child);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(FieldAccess &node) {
    if (node.expression) {
        this->operator()(*node.expression);
    }
    if (node.identifier) {
        this->operator()(*node.identifier);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(ArrayAccess &node) {
    if (node.array) {
        this->operator()(*node.array);
    }
    if (node.selector) {
        this->operator()(*node.selector);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(MethodInvocation &node) {
    if (node.parent_expr) {
        this->operator()(*node.parent_expr);
    }
    if (node.method_name) {
        this->operator()(*node.method_name);
    }
    for (auto &child : node.arguments) {
        this->operator()(child);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(InstanceOfExpression &node) {
    if (node.expression) {
        this->operator()(*node.expression);
    }
    if (node.type) {
        this->operator()(*node.type);
    }
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(ParenthesizedExpression &node) {
    if (node.expression) {
        this->operator()(*node.expression);
    }
}

// Dispatch nested variant to member type
template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(Statement &node) {
    return std::visit( 
        [&](auto &statement_type) { return this->operator()(statement_type); }, 
        node);
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(Expression &node) {
    return std::visit( 
        [&](auto &expression_type) { return this->operator()(expression_type); }, 
        node);
}

template <typename ReturnType>
void AstVisitor<ReturnType>::visit_children(ExpressionStatement &node) {
    return std::visit( 
        [&](auto &expr_stmt_type) { return this->operator()(expr_stmt_type); }, 
        node);
}

template <typename ReturnType>
void AstVisitor<ReturnType>::operator()(Statement &node) {
    this->visit_children(node);
}

template <typename ReturnType>
void AstVisitor<ReturnType>::operator()(Expression &node) {
    this->visit_children(node);
}

template <typename ReturnType>
void AstVisitor<ReturnType>::operator()(ExpressionStatement &node) {
    this->visit_children(node);
}
