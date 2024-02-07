#pragma once

#include "inheritance-ast/ast.h"
#include <iostream>

namespace Inheritance {

struct BinaryExpression;
struct UnaryExpression;
struct Integer;
struct IfStatement;
struct ExpressionStatement;
struct CompilationUnit;

// Abstract AstVisitor that other Visitors implement
struct AstVisitor {
    // Must define visit for each base type
    virtual void visit(BinaryExpression &v) = 0;
    virtual void visit(UnaryExpression &v) = 0;
    virtual void visit(Integer &v) = 0;

    virtual void visit(IfStatement &v) = 0;
    virtual void visit(ExpressionStatement &v) = 0;

    virtual void visit(CompilationUnit &v) = 0;

    virtual ~AstVisitor() = default;
};

// Concrete Visitor
struct PrintVisitor : public AstVisitor {
    virtual void visit(BinaryExpression &v);

    virtual void visit(UnaryExpression &v);

    virtual void visit(Integer &v);

    virtual void visit(ExpressionStatement &v);

    virtual void visit(IfStatement &v);

    virtual void visit(CompilationUnit &v);

};


}
