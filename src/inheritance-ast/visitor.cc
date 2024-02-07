#include "inheritance-ast/ast.h"
#include "inheritance-ast/visitor.h"
#include <iostream>

namespace Inheritance {

// Concrete Visitor
void PrintVisitor::visit(BinaryExpression &v) {
    std::cout << "BinaryExpression(";
    v.lhs->accept(*this);
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
    v.rhs->accept(*this);
    std::cout << ")";
}

void PrintVisitor::visit(UnaryExpression &v) {
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
    v.sub_expression->accept(*this);
    std::cout << ")";
}

void PrintVisitor::visit(Integer &v) {
    std::cout << "Integer(" << v.integer << ")";
}

void PrintVisitor::visit(IfStatement &v) {
    std::cout << "If(";
    v.if_clause->accept(*this);
    std::cout << ", then ";
    v.then_clause->accept(*this);
    std::cout << ")";
}

void PrintVisitor::visit(ExpressionStatement &v) {
    std::cout << "ExpressionStatement(";
    v.statement_expression->accept(*this);
    std::cout << ")";
}

void PrintVisitor::visit(CompilationUnit &v) {
    std::cout << "Program(";
    for (auto &s : v.program_statements) {
        s->accept(*this);
        std::cout << ", ";
    }
    std::cout << ")";
}

}
