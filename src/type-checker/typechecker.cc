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


#include "type-checker/typechecker.h"


void TypeChecker::operator()(Assignment &node) {
    this->visit_children(node);
}

LinkedType LiteralToLinkedType(Literal*& literal) {
    LinkedType linkedType;
    if(std::get_if<int64_t>(literal)) {
        linkedType.linked_type = PrimitiveType::INT;
    }
    else if(std::get_if<bool>(literal)) {
        linkedType.linked_type = PrimitiveType::BOOLEAN;
    }
    else if(std::get_if<char>(literal)) {
        linkedType.linked_type = PrimitiveType::CHAR;
    }
    else if(std::get_if<std::string>(literal)) {
        /* assign String class to linkedType */
    }
    else if(std::get_if<std::nullptr_t>(literal)) {
        linkedType.linked_type = std::nullptr_t{};
    }
    else {
        throw "Invalid literal";
    }
    return linkedType;
}

bool isPrimitiveNumeric(LinkedType& linkedType) {
    if(linkedType.isPrimitive()) {
        auto typeEnum = std::get<PrimitiveType>(linkedType.linked_type);
        if(typeEnum == PrimitiveType::INT || typeEnum == PrimitiveType::SHORT || typeEnum == PrimitiveType::BYTE || 
            typeEnum == PrimitiveType::CHAR) {
            return true;
        }
    }
    return false;
}

bool isBoolean(LinkedType& linkedType) {
    if(linkedType.isPrimitive()) {
        auto typeEnum = std::get<PrimitiveType>(linkedType.linked_type);
        if(typeEnum == PrimitiveType::BOOLEAN) {
            return true;
        }
    }
    return false;
}

void TypeChecker::operator()(InfixExpression &node) {
    this->visit_children(node);

    LinkedType linkedType1;
    Literal* literal1;
    LinkedType linkedType2;
    Literal* literal2;

    if(std::get_if<InfixExpression>(node.expression1.get())) {
        linkedType1 = std::get_if<InfixExpression>(node.expression1.get())->link;
        if(linkedType1.is_array) {
            throw "Array type not allowed in infix expression";
        }
    }
    else if(std::get_if<Literal>(node.expression1.get())) { 
        literal1 = std::get_if<Literal>(node.expression1.get());
        linkedType1 = LiteralToLinkedType(literal1);
    }
    else {
        throw "Invalid expression";
    }

    if(std::get_if<InfixExpression>(node.expression2.get())) {
        linkedType2 = std::get_if<InfixExpression>(node.expression2.get())->link;
        if(linkedType2.is_array) {
            throw "Array type not allowed in infix expression";
        }
    }
    else if(std::get_if<Literal>(node.expression2.get())) {
        literal2 = std::get_if<Literal>(node.expression2.get());
        linkedType2 = LiteralToLinkedType(literal2);
    }
    else {
        throw "Invalid expression";
    }

    switch (node.op) {
        case InfixOperator::PLUS:
            /* if condition for string */
        case InfixOperator::MINUS:
        case InfixOperator::MULTIPLY:
        case InfixOperator::DIVIDE:
        case InfixOperator::MODULO:
            if(isPrimitiveNumeric(linkedType1) && isPrimitiveNumeric(linkedType2)) {
                node.link.linked_type = PrimitiveType::INT;
            }
            else {
                throw "Invalid type for arithmetic operation";
            }
            break;
        case InfixOperator::LESS_THAN:
        case InfixOperator::GREATER_THAN:
        case InfixOperator::LESS_THAN_EQUAL:
        case InfixOperator::GREATER_THAN_EQUAL:
            if(isPrimitiveNumeric(linkedType1) && isPrimitiveNumeric(linkedType2)) {
                node.link.linked_type = PrimitiveType::BOOLEAN;
            }
            else {
                throw "Invalid type for comparison operation";
            }
            break;
        case InfixOperator::BOOLEAN_EQUAL:
        case InfixOperator::BOOLEAN_NOT_EQUAL:
            /* to do */
            break;
        case InfixOperator::BOOLEAN_AND:
        case InfixOperator::BOOLEAN_OR:
        case InfixOperator::EAGER_AND:
        case InfixOperator::EAGER_OR:
            if(isBoolean(linkedType1) && isBoolean(linkedType2)) {
                node.link.linked_type = PrimitiveType::BOOLEAN;
            }
            else {
                throw "Invalid type for boolean operation";
            }
            break;
        default:
            throw "Invalid type for InfixExpression";
    }
}

void TypeChecker::operator()(CastExpression &node) {
    this->visit_children(node);
}

