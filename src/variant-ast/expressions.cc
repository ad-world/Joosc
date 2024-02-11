#include "expressions.h"
#include "names.h"
#include "types.h"

Assignment::Assignment(
    std::unique_ptr<Expression>& assigned_to,
    std::unique_ptr<Expression>& assigned_from
) :
    assigned_to{std::move(assigned_to)},
    assigned_from{std::move(assigned_from)}
{}

QualifiedThis::QualifiedThis(
    std::unique_ptr<QualifiedIdentifier>& qt
) : 
    qualified_this{std::move(qt)} 
{}

ArrayCreationExpression::ArrayCreationExpression(
    std::unique_ptr<Type>& type,
    std::unique_ptr<Expression>& expr
) : 
    type{std::move(type)},
    expression{std::move(expr)} 
{}

ClassInstanceCreationExpression::ClassInstanceCreationExpression(
    std::unique_ptr<QualifiedIdentifier>& class_name,
    std::vector<Expression>& arguments
): 
    class_name{std::move(class_name)},
    arguments{std::move(arguments)} 
{}

FieldAccess::FieldAccess(
    std::unique_ptr<Expression>& expression,
    std::unique_ptr<Identifier>& identifier
): 
    expression{std::move(expression)},
    identifier{std::move(identifier)}
{}

ArrayAccess::ArrayAccess(
    std::unique_ptr<Expression>& array,
    std::unique_ptr<Expression>& selector  
): 
    array{std::move(array)},
    selector{std::move(selector)}
{}

MethodInvocation::MethodInvocation(
    std::unique_ptr<Expression>& method_name,
    std::vector<Expression>& arguments
):
    method_name{std::move(method_name)},
    arguments{std::move(arguments)} 
{}

InfixExpression::InfixExpression(
    std::unique_ptr<Expression>& ex1,
    std::unique_ptr<Expression>& ex2,
    InfixOperator op
) :
    expression1{std::move(ex1)},
    expression2{std::move(ex2)},
    op{op}
{}

PrefixExpression::PrefixExpression(
    std::unique_ptr<Expression>& expression,
    PrefixOperator op
): 
    expression{std::move(expression)},
    op{op}
{}

CastExpression::CastExpression(
    std::unique_ptr<Type>& type,
    std::unique_ptr<Expression>& expression
): 
    type{std::move(type)},
    expression{std::move(expression)}
{}
