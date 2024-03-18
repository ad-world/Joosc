#include "local-variable-visitor.h"
#include "exceptions/exceptions.h"
#include "utillities/util.h"
#include "reachability/reachability.h"
#include "type-checker/typechecker.h"

void LocalVariableVisitor::operator()(MethodDeclaration &node) {
    current_method = node.environment;
    this->visit_children(node);
    current_method = nullptr;
}

void LocalVariableVisitor::checkExpressionForIdentifier(Expression &expr, Identifier &identifier) {
    std::visit(util::overload {
        [&](QualifiedIdentifier &node) {
            if (node.identifiers.size() == 1) {
                if (node.identifiers.front().name == identifier.name) {
                    THROW_LocalVariableError("Local variable " + identifier.name + " has itself in the initializer at: " + Util::locationToString(node.location));
                }
            }
        },
        [&](Literal &node) {},
        [&](MethodInvocation &node) {
            for (auto &arg: node.arguments)  checkExpressionForIdentifier(arg, identifier);
        },
        [&](ArrayAccess &node) {
            checkExpressionForIdentifier(*node.array, identifier);
            checkExpressionForIdentifier(*node.selector, identifier);
        },
        [&](ClassInstanceCreationExpression &node) {
            for (auto &arg: node.arguments)  checkExpressionForIdentifier(arg, identifier);
        },
        [&](InfixExpression &node) {
            checkExpressionForIdentifier(*node.expression1, identifier);
            checkExpressionForIdentifier(*node.expression2, identifier);
        },
        [&](Assignment &node) {
            checkExpressionForIdentifier(*node.assigned_from, identifier);
            checkExpressionForIdentifier(*node.assigned_to, identifier);
        },
        [&](QualifiedThis &node) {},
        [&](auto &node) {
            checkExpressionForIdentifier(*node.expression, identifier);
        }
    }, expr);   
}

LinkedType LocalVariableVisitor::getLink(Expression &node) {
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
                    NonArrayLinkedType string_type = Util::root_package->findClassDeclaration("java.lang.String");
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


void LocalVariableVisitor::operator()(LocalVariableDeclaration &node) {
    auto &expr = node.variable_declarator->expression;
    if (expr == nullptr) {
        std::string location = Util::locationToString(node.location);

        THROW_LocalVariableError("Local variable " + node.variable_declarator->variable_name->name + " must be initialized at " + location);
    } else {
        checkExpressionForIdentifier(*expr, *node.variable_declarator->variable_name);

        auto linked_expr_type = TypeChecker::getLink(*expr);

        if (node.type->link.isPrimitive() && linked_expr_type.isPrimitive()) {
            auto node_type_primitive = node.type->link.getIfIsPrimitive();
            auto expr_type_primitive = linked_expr_type.getIfIsPrimitive();

            switch (*expr_type_primitive) {
                case PrimitiveType::INT:
                    // Case 1: assigning int to byte or short or char
                    switch (*node_type_primitive) {
                        case PrimitiveType::BYTE:
                        case PrimitiveType::SHORT:
                        case PrimitiveType::CHAR:
                            THROW_LocalVariableError("Narrowing conversion from int to " + node.type->link.toSimpleString() + " in local variable declaration at: " + Util::locationToString(node.location));
                        default:
                            break;
                    }
                    break;
                case PrimitiveType::BYTE:
                    // Case 2: assigning byte to a char
                    if (*node_type_primitive == PrimitiveType::CHAR) {
                        THROW_LocalVariableError("Narrowing conversion from byte to " + node.type->link.toSimpleString() + " in local variable declaration at: " + Util::locationToString(node.location));
                    }
                    break;
                case PrimitiveType::SHORT:
                    // Case 3: assigning short to a char or byte
                    switch (*node_type_primitive) {
                        case PrimitiveType::BYTE:
                        case PrimitiveType::CHAR:
                            THROW_LocalVariableError("Narrowing conversion from short to char in local variable declaration at: " + Util::locationToString(node.location));
                        default:
                            break;
                    }
                    break;
                case PrimitiveType::CHAR:
                    // Case 4: assigning char to byte or short
                    switch (*node_type_primitive) {
                        case PrimitiveType::BYTE:
                        case PrimitiveType::SHORT:
                            THROW_LocalVariableError("Narrowing conversion from char to " + node.type->link.toSimpleString() + " in local variable declaration at: " + Util::locationToString(node.location));
                        default:
                            break;
                    }
                    break;
            }
        }
    }
}