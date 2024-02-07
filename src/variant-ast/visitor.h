#pragma once

#include "variant-ast/ast.h"
#include <iostream>

namespace Variant {

// struct AstNode;
struct BinaryExpression;
struct UnaryExpression;
struct Integer;
struct IfStatement;
struct ExpressionStatement;
struct CompilationUnit;

// template <typename ReturnType>
// struct AstVisitor {
//     ReturnType visit(AstNode& a) {
//         return std::visit(*this, a);
//     }
//     virtual ~AstVisitor() = default;
// };

// Concrete Visitor
struct PrintVisitor {
    void operator()(BinaryExpression &v);

    void operator()(UnaryExpression &v);

    void operator()(Integer &v);

    void operator()(ExpressionStatement &v);

    void operator()(IfStatement &v);

    void operator()(CompilationUnit &v);

    // Void could really be any type; could have different type per visitor
    void visit(AstNode& a) {
        return std::visit(*this, a);
    }
};


}
