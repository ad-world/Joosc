#pragma once

#include <memory>
#include <vector>
#include <variant>

namespace Variant {

// The variant definition using all defined classes (this would be in its own file)
struct BinaryExpression;
struct UnaryExpression;
struct Integer;
struct IfStatement;
struct ExpressionStatement;
struct CompilationUnit;

typedef std::variant<
    // Expressions
    BinaryExpression,
    UnaryExpression,
    Integer,
    // Statements
    IfStatement,
    ExpressionStatement,
    CompilationUnit
> AstNode;

// For putting common decorator information e.g. Type
struct AstNodeCommon {
    virtual ~AstNodeCommon() = default;
};

struct BinaryExpression : public AstNodeCommon {
    enum BinaryOperator {
        PLUS, MINUS, TIMES, DIVIDE, 
    };
    BinaryOperator op;
    std::unique_ptr<AstNode> lhs;
    std::unique_ptr<AstNode> rhs;
};

struct UnaryExpression : public AstNodeCommon {
    enum UnaryOperator {
        NEGATE, // Boolean negate
        MINUS, // Number sign
    };
    UnaryOperator op;
    std::unique_ptr<AstNode> sub_expression;
};

struct Integer : public AstNodeCommon {
    int integer;
    Integer(int a) {this->integer = a;}
};

struct IfStatement : public AstNodeCommon {
    std::unique_ptr<AstNode> if_clause;
    std::unique_ptr<AstNode> then_clause;
};

struct ExpressionStatement : public AstNodeCommon {
    std::unique_ptr<AstNode> statement_expression;
};

struct CompilationUnit: public AstNodeCommon {
    // std::vector<std::unique_ptr<AstNode>> program_statements; // this would be replaced with package, types, etc
    std::vector<AstNode> program_statements; // this would be replaced with package, types, etc
};

}
