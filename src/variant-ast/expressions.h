#pragma once

#include <variant>
#include <memory>
#include <string>
#include <vector>
#include "astnodecommon.h"
#include "type-decl/linkedtype.h"

struct Identifier;
struct QualifiedIdentifier;
struct Type;

enum class InfixOperator {
    BOOLEAN_OR,
    BOOLEAN_AND,
    EAGER_OR,
    EAGER_AND,
    BOOLEAN_EQUAL,
    BOOLEAN_NOT_EQUAL,
    PLUS,
    MINUS,
    DIVIDE,
    MULTIPLY,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    MODULO
};

enum class PrefixOperator {
    MINUS,
    NEGATE
};

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
    struct ArrayCreationExpression,
    struct QualifiedIdentifier,
    struct InstanceOfExpression,
    struct ParenthesizedExpression
> Expression;

// Shared fields for all expression subtypes
struct ExpressionCommon: public AstNodeCommon {
    LinkedType link; // The compile-time type of the expression; resolved in type-linking & type-checking
    bool is_variable; // Whether is expression is an assignable variable; the Java equivalent to a C++ lvalue
};

struct Assignment: public ExpressionCommon {
    // Represents assigned_to = assigned_from
    std::unique_ptr<Expression> assigned_to;
    std::unique_ptr<Expression> assigned_from;

    Assignment(
        std::unique_ptr<Expression>& assigned_to,
        std::unique_ptr<Expression>& assigned_from
    );
    Assignment(
        std::unique_ptr<Expression>&& assigned_to,
        std::unique_ptr<Expression>&& assigned_from
    );
};

struct QualifiedThis: public ExpressionCommon {
    std::unique_ptr<QualifiedIdentifier> qualified_this;

    QualifiedThis(
        std::unique_ptr<QualifiedIdentifier>& qt
    );
    QualifiedThis(
        std::unique_ptr<QualifiedIdentifier>&& qt
    );
};

struct ArrayCreationExpression: public ExpressionCommon {
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> expression;

    ArrayCreationExpression(
        std::unique_ptr<Type>& type,
        std::unique_ptr<Expression>& expr
    );
    ArrayCreationExpression(
        std::unique_ptr<Type>&& type,
        std::unique_ptr<Expression>&& expr
    );
};

struct ClassInstanceCreationExpression: public ExpressionCommon {
    std::unique_ptr<QualifiedIdentifier> class_name;
    std::vector<Expression> arguments;

    ClassInstanceCreationExpression(
        std::unique_ptr<QualifiedIdentifier>& class_name,
        std::vector<Expression>& arguments
    );
    ClassInstanceCreationExpression(
        std::unique_ptr<QualifiedIdentifier>&& class_name,
        std::vector<Expression>&& arguments
    );
};

struct FieldAccess: public ExpressionCommon {
    std::unique_ptr<Expression> expression;
    std::unique_ptr<Identifier> identifier;

    FieldAccess(
        std::unique_ptr<Expression>& expression,
        std::unique_ptr<Identifier>& identifier
    );
    FieldAccess(
        std::unique_ptr<Expression>&& expression,
        std::unique_ptr<Identifier>&& identifier
    );
};

struct ArrayAccess: public ExpressionCommon {
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> selector;

    ArrayAccess(
        std::unique_ptr<Expression>& array,
        std::unique_ptr<Expression>& selector  
    );
    ArrayAccess(
        std::unique_ptr<Expression>&& array,
        std::unique_ptr<Expression>&& selector  
    );
};

struct MethodInvocation: public ExpressionCommon {
    std::unique_ptr<Expression> parent_expr;    // CAN BE NULL
    std::unique_ptr<Identifier> method_name;
    std::vector<Expression> arguments;

    MethodInvocation(
        std::unique_ptr<Expression>& parent_expr,
        std::unique_ptr<Identifier>& method_name,
        std::vector<Expression>& arguments
    );
    MethodInvocation(
        std::unique_ptr<Expression>&& parent_expr,
        std::unique_ptr<Identifier>&& method_name,
        std::vector<Expression>&& arguments
    );
};

struct InfixExpression : public ExpressionCommon {
    std::unique_ptr<Expression> expression1;
    std::unique_ptr<Expression> expression2;
    InfixOperator op;

    InfixExpression(
        std::unique_ptr<Expression>& ex1,
        std::unique_ptr<Expression>& ex2,
        InfixOperator op
    );
    InfixExpression(
        std::unique_ptr<Expression>&& ex1,
        std::unique_ptr<Expression>&& ex2,
        InfixOperator op
    );
};

struct PrefixExpression : public ExpressionCommon {
    std::unique_ptr<Expression> expression;
    PrefixOperator op;

    PrefixExpression(
        std::unique_ptr<Expression>& expression,
        PrefixOperator op
    );
    PrefixExpression(
        std::unique_ptr<Expression>&& expression,
        PrefixOperator op
    );
};

struct CastExpression : public ExpressionCommon {
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> expression;

    CastExpression(
        std::unique_ptr<Type>& type,
        std::unique_ptr<Expression>& expression
    );
    CastExpression(
        std::unique_ptr<Type>&& type,
        std::unique_ptr<Expression>&& expression
    );
};

struct InstanceOfExpression : public ExpressionCommon {
    std::unique_ptr<Expression> expression;
    std::unique_ptr<Type> type;

    InstanceOfExpression(
        std::unique_ptr<Expression>& expression,
        std::unique_ptr<Type>& type
    );
    InstanceOfExpression(
        std::unique_ptr<Expression>&& expression,
        std::unique_ptr<Type>&& type
    );
};

struct ParenthesizedExpression : public ExpressionCommon {
    std::unique_ptr<Expression> expression;
    
    ParenthesizedExpression(
        std::unique_ptr<Expression>& expression
    );
    ParenthesizedExpression(
        std::unique_ptr<Expression>&& expression
    );
};
