// #pragma once

// #include "variant-ast/ast.h"
// #include <iostream>

// namespace Variant {

// // struct AstNode;
// struct BinaryExpression;
// struct UnaryExpression;
// struct Integer;
// struct IfStatement;
// struct ExpressionStatement;
// struct CompilationUnit;

// // Superclass for visitor operation that returns a specific type
// template <typename SubReturnType, typename ReturnType>
// struct AstVisitor {
//     virtual SubReturnType operator()(BinaryExpression &v) = 0;

//     virtual SubReturnType operator()(UnaryExpression &v) = 0;

//     virtual SubReturnType operator()(Integer &v) = 0;

//     virtual SubReturnType operator()(ExpressionStatement &v) = 0;

//     virtual SubReturnType operator()(IfStatement &v) = 0;

//     virtual SubReturnType operator()(CompilationUnit &v) = 0;

//     virtual ReturnType visit(AstNode& a) = 0;
//     virtual ~AstVisitor() = default;
// };

// // Concrete Print Visitor
// struct PrintVisitor : public AstVisitor<void, void> {
//     void operator()(BinaryExpression &v) override;

//     void operator()(UnaryExpression &v) override;

//     void operator()(Integer &v) override;

//     void operator()(ExpressionStatement &v) override;

//     void operator()(IfStatement &v) override;

//     void operator()(CompilationUnit &v) override;

//     void visit(AstNode& a) override {
//         return std::visit(*this, a);
//     }
// };

// // Concrete "Pass through" Visitor that allows subclasses that don't define behaviour for types they don't touch
// template <typename SubReturnType, typename ReturnType>
// struct PassThroughVisitor : public AstVisitor<SubReturnType, ReturnType> {
    
//     virtual SubReturnType getDefaultValue() {
//         SubReturnType default_value;
//         return default_value;
//     }

//     virtual SubReturnType operator()(BinaryExpression &v) override {
//         visit(v.lhs);
//         visit(v.rhs);
//         return getDefaultValue();
//     }

//     virtual SubReturnType operator()(UnaryExpression &v) override {
//         visit(v.sub_expression);
//         return getDefaultValue();
//     }

//     virtual SubReturnType operator()(Integer &v) override {
//         return getDefaultValue();
//     }

//     virtual SubReturnType operator()(ExpressionStatement &v) override {
//         visit(v.statement_expression);
//         return getDefaultValue();
//     }

//     virtual SubReturnType operator()(IfStatement &v) override {
//         visit(v.if_clause);
//         visit(v.then_clause);
//         return getDefaultValue();
//     }

//     virtual SubReturnType operator()(CompilationUnit &v) override {
//         // for (auto &p: v.program_statements) {
//         //     visit(v.program_statements);
//         // }
//         return getDefaultValue();
//     }

//     virtual ReturnType visit(AstNode& a) = 0;
// };

// }
