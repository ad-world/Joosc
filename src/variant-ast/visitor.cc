#include "variant-ast/ast.h"
#include "variant-ast/visitor.h"
#include <iostream>

namespace Variant {

// Concrete Visitor
void PrintVisitor::operator()(BinaryExpression &v) {
    std::cout << "BinaryExpression(";
    visit(*v.lhs);
    switch (v.op) {
        case BinaryExpression::BinaryOperator::PLUS:
            std::cout << " + ";
            break;
        case BinaryExpression::BinaryOperator::MINUS:
            std::cout << " - ";
            break;
        case BinaryExpression::BinaryOperator::TIMES:
            std::cout << " * ";
            break;
        case BinaryExpression::BinaryOperator::DIVIDE:
            std::cout << " / ";
            break;
        default:
            break;
    }
    visit(*v.rhs);
    std::cout << ")";
}

void PrintVisitor::operator()(UnaryExpression &v) {
    std::cout << "UnaryExpression(";
    switch (v.op) {
        case UnaryExpression::UnaryOperator::MINUS:
            std::cout << "-";
            break;
        case UnaryExpression::UnaryOperator::NEGATE:
            std::cout << "!";
        default:
            break;
    }
    visit(*v.sub_expression);
    std::cout << ")";
}

void PrintVisitor::operator()(Integer &v) {
    std::cout << "Integer(" << v.integer << ")";
}

void PrintVisitor::operator()(IfStatement &v) {
    std::cout << "If(";
    visit(*v.if_clause);
    std::cout << ", then ";
    visit(*v.then_clause);
    std::cout << ")";
}

void PrintVisitor::operator()(ExpressionStatement &v) {
    std::cout << "ExpressionStatement(";
    visit(*v.statement_expression);
    std::cout << ")";
}

void PrintVisitor::operator()(CompilationUnit &v) {
    std::cout << "Program(";
    for (auto &s : v.program_statements) {
        visit(s);
        std::cout << ", ";
    }
    std::cout << ")";
}

}
