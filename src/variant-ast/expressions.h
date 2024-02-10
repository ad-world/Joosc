#pragma once

#include <variant>
#include <memory>
#include <string>

typedef std::variant<int64_t, bool, char, std::string, std::nullptr_t> Literal;

typedef std::variant<
    struct Assignment,
    struct InfixExpression,
    struct PrefixExpression,
    struct CastExpression,
    Literal,
    struct ClassInstanceCreationExpression,
    struct FieldAccess,
    struct MethodInvocation,
    struct ArrayAccess,
    struct QualifiedThis,
    struct ArrayCreationExpression
> Expression;

struct Assignment: public AstNodeCommon {
    // Represents assigned_to = assigned_from
    std::unique_ptr<Expression> assigned_to;
    std::unique_ptr<Expression> assigned_from;

    Assignment(
        std::unique_ptr<Expression>& assigned_to,
        std::unique_ptr<Expression>& assigned_from
    );
};
