#include "type-checker/typechecker.h"



void TypeChecker::operator()(Assignment &node) {
    this->visit_children(node);
}

void TypeChecker::operator()(InfixExpression &node) {
    this->visit_children(node);

    switch (node.op) {
        case InfixOperator::PLUS:
        case InfixOperator::MINUS:
        case InfixOperator::MULTIPLY:
        case InfixOperator::DIVIDE:
        case InfixOperator::MODULO:
        case InfixOperator::LESS_THAN:
        case InfixOperator::GREATER_THAN:
        case InfixOperator::LESS_THAN_EQUAL:
        case InfixOperator::GREATER_THAN_EQUAL:
        case InfixOperator::BOOLEAN_EQUAL:
        case InfixOperator::BOOLEAN_NOT_EQUAL:
        case InfixOperator::BOOLEAN_AND:
        case InfixOperator::BOOLEAN_OR:
        case InfixOperator::EAGER_AND:
        case InfixOperator::EAGER_OR:
    }
}

void TypeChecker::operator()(CastExpression &node) {
    this->visit_children(node);
}

