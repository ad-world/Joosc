#pragma once

#include <memory>
#include <vector>
#include "inheritance-ast/visitor.h"

namespace Inheritance {

// AstNode groupings/types for this example:
// Expression (BinaryExpression, UnaryExpression, Integer), 
// Statement (IfStatement, ExpressionStatement)
// CompilationUnit

struct AstNode {
    virtual void accept(AstVisitor &v) = 0;
    virtual ~AstNode() = default;
};

// Abstract Expression class groups together subtypes
struct Expression : public AstNode {
    virtual void accept(AstVisitor &v) = 0;
};

struct BinaryExpression : public Expression {
    enum BinaryOperator {
        PLUS, MINUS, TIMES, DIVIDE, 
    };
    BinaryOperator op;
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    virtual void accept(AstVisitor &v) override;
};

struct UnaryExpression : public Expression {
    enum UnaryOperator {
        NEGATE, // Boolean negate
        MINUS, // Number sign
    };
    UnaryOperator op;
    std::unique_ptr<Expression> sub_expression;
    virtual void accept(AstVisitor &v) override;
};

struct Integer : public Expression {
    int integer;
    virtual void accept(AstVisitor &v) override;
    Integer(int a) {this->integer = a;}
};

// Abstract Statement class groups together subtypes
struct Statement : public AstNode {
    virtual void accept(AstVisitor &v) = 0;
};

struct IfStatement : public Statement {
    std::unique_ptr<Expression> if_clause;
    std::unique_ptr<Statement> then_clause;
    virtual void accept(AstVisitor &v) override;
};

struct ExpressionStatement : public Statement {
    std::unique_ptr<Expression> statement_expression;
    virtual void accept(AstVisitor &v) override;
};

// CompilationUnit; not grouped into anything, directly inherits from AstNode
struct CompilationUnit: public AstNode {
    std::vector<std::unique_ptr<Statement>> program_statements; // this would be replaced with package, types, etc
    virtual void accept(AstVisitor &v) override;
};


}
