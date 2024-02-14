#include "statements.h"
#include "classes.h"
#include "types.h"
#include "expressions.h"
#include "names.h"

IfThenStatement::IfThenStatement(
    std::unique_ptr<Expression>& if_clause,
    std::unique_ptr<Statement>& then_clause
) : 
    if_clause{std::move(if_clause)},
    then_clause{std::move(then_clause)} 
{}

IfThenElseStatement::IfThenElseStatement(
    std::unique_ptr<Expression>& if_clause,
    std::unique_ptr<Statement>& then_clause,
    std::unique_ptr<Statement>& else_clause
) : 
    if_clause{std::move(if_clause)},
    then_clause{std::move(then_clause)},
    else_clause{std::move(else_clause)}
{}

WhileStatement::WhileStatement(
    std::unique_ptr<Expression>& condition_expression,
    std::unique_ptr<Statement>& body_statement
) : 
    condition_expression{std::move(condition_expression)},
    body_statement{std::move(body_statement)}
{}

ForStatement::ForStatement(
    std::unique_ptr<Statement>& init_statement,
    std::unique_ptr<Expression>& condition_expression,
    std::unique_ptr<Statement>& update_statement,
    std::unique_ptr<Statement>& body_statement
) : 
    init_statement{std::move(init_statement)},
    condition_expression{std::move(condition_expression)},
    update_statement{std::move(update_statement)},
    body_statement{std::move(body_statement)}
{}

Block::Block(
    std::vector<LocalVariableDeclaration>& variable_declarations, 
    std::vector<Statement>& statements
) :
    variable_declarations{std::move(variable_declarations)}, 
    statements{std::move(statements)}
{}

ReturnStatement::ReturnStatement(
    std::unique_ptr<Expression>& return_expression
) : 
    return_expression{std::move(return_expression)}
{}

LocalVariableDeclaration::LocalVariableDeclaration(
    std::unique_ptr<Type>& type,
    std::unique_ptr<VariableDeclarator>& variable_declarator
) : 
    type(std::move(type)),
    variable_declarator(std::move(variable_declarator))
{}

// -------------

IfThenStatement::IfThenStatement(
    std::unique_ptr<Expression>&& if_clause,
    std::unique_ptr<Statement>&& then_clause
) : 
    if_clause{std::move(if_clause)},
    then_clause{std::move(then_clause)} 
{}

IfThenElseStatement::IfThenElseStatement(
    std::unique_ptr<Expression>&& if_clause,
    std::unique_ptr<Statement>&& then_clause,
    std::unique_ptr<Statement>&& else_clause
) : 
    if_clause{std::move(if_clause)},
    then_clause{std::move(then_clause)},
    else_clause{std::move(else_clause)}
{}

WhileStatement::WhileStatement(
    std::unique_ptr<Expression>&& condition_expression,
    std::unique_ptr<Statement>&& body_statement
) : 
    condition_expression{std::move(condition_expression)},
    body_statement{std::move(body_statement)}
{}

ForStatement::ForStatement(
    std::unique_ptr<Statement>&& init_statement,
    std::unique_ptr<Expression>&& condition_expression,
    std::unique_ptr<Statement>&& update_statement,
    std::unique_ptr<Statement>&& body_statement
) : 
    init_statement{std::move(init_statement)},
    condition_expression{std::move(condition_expression)},
    update_statement{std::move(update_statement)},
    body_statement{std::move(body_statement)}
{}

Block::Block(
    std::vector<LocalVariableDeclaration>&& variable_declarations, 
    std::vector<Statement>&& statements
) :
    variable_declarations{std::move(variable_declarations)}, 
    statements{std::move(statements)}
{}

ReturnStatement::ReturnStatement(
    std::unique_ptr<Expression>&& return_expression
) : 
    return_expression{std::move(return_expression)}
{}

LocalVariableDeclaration::LocalVariableDeclaration(
    std::unique_ptr<Type>&& type,
    std::unique_ptr<VariableDeclarator>&& variable_declarator
) : 
    type(std::move(type)),
    variable_declarator(std::move(variable_declarator))
{}
