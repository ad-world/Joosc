#include "type-checker/typechecker.h"
#include "utillities/util.h"

LinkedType TypeChecker::getLink(Expression &node) {
    return std::visit(util::overload {
        // Literal handled seperately as it is not a class
        [&] (Literal expr_type) -> LinkedType {
            return std::visit(util::overload {
                [&] (int64_t literal_type) -> LinkedType {
                    return LinkedType(PrimitiveType::INT);
                },
                [&] (bool literal_type) -> LinkedType {
                    return LinkedType(PrimitiveType::BOOLEAN);
                },
                [&] (char literal_type) -> LinkedType {
                    return LinkedType(PrimitiveType::CHAR);
                },
                [&] (std::string& literal_type) -> LinkedType {
                    NonArrayLinkedType string_type = default_package->getJavaLangObject();
                    return LinkedType(string_type);
                },
                [&] (std::nullptr_t literal_type) -> LinkedType {
                    return LinkedType(PrimitiveType::NULL_T);
                },
            }, expr_type);
        },
        // Any non-literal subexpression
        [] (auto& expr_type) -> LinkedType {
            return expr_type.link;
        }
    }, node);
}

LinkedType TypeChecker::getLink(std::unique_ptr<Expression>& node_ptr) {
    return getLink(*node_ptr);
}

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
            break;
    };
}

void TypeChecker::operator()(CastExpression &node) {
    this->visit_children(node);
}

