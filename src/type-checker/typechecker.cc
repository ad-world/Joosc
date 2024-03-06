#include "type-checker/typechecker.h"
#include "exceptions/exceptions.h"

void TypeChecker::operator()(InfixExpression &node) {
    this->visit_children(node);

    LinkedType linkedType1 = getLink(node.expression1);
    LinkedType linkedType2 = getLink(node.expression1);
    if(linkedType1.is_array || linkedType2.is_array) {
        THROW_TypeCheckerError("Infix operators are not defined for array types");
    }

    switch (node.op) {
        case InfixOperator::PLUS:
            if(linkedType1.isNumeric() && linkedType2.isNumeric()) {
                node.link.linked_type = PrimitiveType::INT;
            }
            else if((linkedType1.isString() && !linkedType2.isVoid()) || (linkedType2.isString() && !linkedType1.isVoid())) {
                /* link String class */
            }
            else {
                THROW_TypeCheckerError("Invalid type for arithmetic operation");
            }
            break;
        case InfixOperator::MINUS:
        case InfixOperator::MULTIPLY:
        case InfixOperator::DIVIDE:
        case InfixOperator::MODULO:
            if(linkedType1.isNumeric() && linkedType2.isNumeric()) {
                node.link.linked_type = PrimitiveType::INT;
            }
            else {
                THROW_TypeCheckerError("Invalid type for arithmetic operation");
            }
            break;
        case InfixOperator::LESS_THAN:
        case InfixOperator::GREATER_THAN:
        case InfixOperator::LESS_THAN_EQUAL:
        case InfixOperator::GREATER_THAN_EQUAL:
            if(linkedType1.isNumeric() && linkedType2.isNumeric()) {
                node.link.linked_type = PrimitiveType::BOOLEAN;
            }
            else {
                THROW_TypeCheckerError("Invalid type for comparison operation");
            }
            break;
        case InfixOperator::BOOLEAN_EQUAL:
        case InfixOperator::BOOLEAN_NOT_EQUAL:
            if(linkedType1 == linkedType2 || (linkedType1.isNull() && (!linkedType2.isPrimitive() || linkedType2.is_array)) 
                || (linkedType2.isNull() && (!linkedType1.isPrimitive() || linkedType1.is_array))) {
                node.link.linked_type = PrimitiveType::BOOLEAN; // Might need to add check for subclasses and base class comparison
            }
            else {
                THROW_TypeCheckerError("Invalid type for comparison operation");
            }
            break;
        case InfixOperator::BOOLEAN_AND:
        case InfixOperator::BOOLEAN_OR:
        case InfixOperator::EAGER_AND:
        case InfixOperator::EAGER_OR:
            if(linkedType1.isBoolean() && linkedType2.isBoolean()) {
                node.link.linked_type = PrimitiveType::BOOLEAN;
            }
            else {
                THROW_TypeCheckerError("Invalid type for boolean operation");
            }
            break;
        default:
            THROW_TypeCheckerError("Invalid type for InfixExpression");
    }
}

void TypeChecker::operator()(PrefixExpression &node) {
    this->visit_children(node);

    LinkedType linkedType = getLink(node.expression);
    if(linkedType.is_array) {
        THROW_TypeCheckerError("Prefix operators are not defined for array types");
    }

    switch (node.op) {
        case PrefixOperator::MINUS:
            if(linkedType.isNumeric()) {
                node.link.linked_type = linkedType.linked_type;
            }
            else {
                THROW_TypeCheckerError("Invalid type for arithmetic operation");
            }
            break;
        case PrefixOperator::NEGATE:
            if(linkedType.isBoolean()) {
                node.link.linked_type = linkedType.linked_type;
            }
            else {
                THROW_TypeCheckerError("Invalid type for boolean operation");
            }
            break;
        default:
            THROW_TypeCheckerError("Invalid type for PrefixExpression");
    }
}

void TypeChecker::operator()(QualifiedThis &node) {
    this->visit_children(node);
    node.link = node.qualified_this.get()->link;
}

void TypeChecker::operator()(ArrayCreationExpression &node) {
    this->visit_children(node);

    LinkedType linkedType = getLink(node.expression);
    if(linkedType.isNumeric()) {
        node.link.linked_type = linkedType.linked_type;
        node.link.is_array = true;
    }
    else {
        THROW_TypeCheckerError("Invalid type for ArrayCreationExpression");
    }
}

void TypeChecker::operator()(ClassInstanceCreationExpression &node) {
    this->visit_children(node);
    node.link = node.linked_class_type;
}

void TypeChecker::operator()(ArrayAccess &node) {
    this->visit_children(node);

    LinkedType typeArray = getLink(node.array);
    LinkedType typeSelector = getLink(node.selector);
    if(typeArray.is_array && typeSelector.isNumeric()) {
        node.link.linked_type = typeArray.linked_type;
    }
    else {
        THROW_TypeCheckerError("Invalid type for ArrayAccess");
    }
}