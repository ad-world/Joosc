#pragma once

#include <variant>
#include <memory>
#include <vector>
#include "astnodecommon.h"
#include "expressions.h"

struct Type;
struct VariableDeclarator;

typedef std::monostate EmptyStatement;

typedef std::variant<
    struct Assignment,
    struct MethodInvocation,
    struct ClassInstanceCreationExpression
> ExpressionStatement;

typedef std::variant<
    struct IfThenStatement,
    struct IfThenElseStatement,
    struct WhileStatement,
    struct ForStatement,
    struct Block,
    EmptyStatement,
    ExpressionStatement,
    struct ReturnStatement,
    struct LocalVariableDeclaration
> Statement;

struct IfThenStatement: public AstNodeCommon {
    std::unique_ptr<Expression> if_clause;
    std::unique_ptr<Statement> then_clause;

    IfThenStatement(
        std::unique_ptr<Expression>& if_clause,
        std::unique_ptr<Statement>& then_clause
    );
    IfThenStatement(
        std::unique_ptr<Expression>&& if_clause,
        std::unique_ptr<Statement>&& then_clause
    );
};

struct IfThenElseStatement: public AstNodeCommon {
    std::unique_ptr<Expression> if_clause;
    std::unique_ptr<Statement> then_clause;
    std::unique_ptr<Statement> else_clause;

    IfThenElseStatement(
        std::unique_ptr<Expression>& if_clause,
        std::unique_ptr<Statement>& then_clause,
        std::unique_ptr<Statement>& else_clause
    );
    IfThenElseStatement(
        std::unique_ptr<Expression>&& if_clause,
        std::unique_ptr<Statement>&& then_clause,
        std::unique_ptr<Statement>&& else_clause
    );
};

struct WhileStatement: public AstNodeCommon {
    std::unique_ptr<Expression> condition_expression;
    std::unique_ptr<Statement> body_statement;

    WhileStatement(
        std::unique_ptr<Expression>& condition_expression,
        std::unique_ptr<Statement>& body_statement
    );
    WhileStatement(
        std::unique_ptr<Expression>&& condition_expression,
        std::unique_ptr<Statement>&& body_statement
    );
};

struct ForStatement: public AstNodeCommon {
    std::unique_ptr<Statement> init_statement;
    std::unique_ptr<Expression> condition_expression;
    std::unique_ptr<Statement> update_statement;
    std::unique_ptr<Statement> body_statement;

    ForStatement(
        std::unique_ptr<Statement>& init_statement,
        std::unique_ptr<Expression>& condition_expression,
        std::unique_ptr<Statement>& update_statement,
        std::unique_ptr<Statement>& body_statement
    );
    ForStatement(
        std::unique_ptr<Statement>&& init_statement,
        std::unique_ptr<Expression>&& condition_expression,
        std::unique_ptr<Statement>&& update_statement,
        std::unique_ptr<Statement>&& body_statement
    );
};

struct Block: public AstNodeCommon {
    std::vector<Statement> statements;

    Block(
        std::vector<Statement>& statements
    );
    Block(
        std::vector<LocalVariableDeclaration>&& useless,
        std::vector<Statement>&& statements
    );
};

struct ReturnStatement: public AstNodeCommon {
    std::unique_ptr<Expression> return_expression;

    ReturnStatement(
        std::unique_ptr<Expression>& return_expression
    );
    ReturnStatement(
        std::unique_ptr<Expression>&& return_expression
    );
};

struct LocalVariableDeclaration: public AstNodeCommon {
    std::unique_ptr<Type> type;
    std::unique_ptr<VariableDeclarator> variable_declarator;

    struct LocalVariableDeclarationObject *environment;

    LocalVariableDeclaration(
        std::unique_ptr<Type>& type,
        std::unique_ptr<VariableDeclarator>& variable_declarator
    );
    LocalVariableDeclaration(
        std::unique_ptr<Type>&& type,
        std::unique_ptr<VariableDeclarator>&& variable_declarator
    );
};
