#include "add-location/add-location.h"
#include "util.h"
PackageDeclarationObject* Util::root_package = nullptr;
std::vector<AstNodeVariant>* Util::linked_asts = nullptr;

std::string Util::locationToString(yy::location &loc) {
    return AddLocation::getString(loc);
}


std::string Util::statementToLocationString(Statement &stmt) {
    yy::location *location = nullptr;

    std::visit(util::overload{
        [&](IfThenStatement &node) -> void {
            location = &node.location;
        },
        [&](IfThenElseStatement &node) -> void {
            location = &node.location;
        },
        [&](WhileStatement &node) -> void {
            location = &node.location;
        },
        [&](ForStatement &node) -> void {
            location = &node.location;
        },
        [&](Block &node) -> void {
            location = &node.location;
        },
        [&](ExpressionStatement &node) -> void {
            std::visit(util::overload{
                [&](auto &node) -> void {
                    location = &node.location;
                }
            }, node);
        },
        [&](std::monostate &node) -> void {},
        // Default case for all other statement types
        [&](auto &node) -> void {
            location = &node.location;
        },
     }, stmt);

    std::string location_str;
    if ( location ) {
        location_str = Util::locationToString(*location);
    }
    return location_str;
}

std::string Util::expressionToLocationString(Expression &expr) {
    yy::location *location = nullptr;

    std::visit(util::overload{
        [&](Literal &node) { /* pass */ },
        [&](auto &node) {
            location = &node.location;
        }
    }, expr);

    std::string location_str;
    if ( location ) {
        location_str = Util::locationToString(*location);
    }
    return location_str;
}

bool Util::checkExpressionForIdentifier(Expression &expr, Identifier &identifier) {
     std::visit(util::overload {
        [&](QualifiedIdentifier &node) {
            if (node.identifiers.size() == 1) {
                if (node.identifiers.front().name == identifier.name) {
                    return true;
                }
            }

            return false;
        },
        [&](Literal &node) { return false; },
        [&](MethodInvocation &node) {
            bool flag = false;
            for (auto &arg: node.arguments) flag = flag || checkExpressionForIdentifier(arg, identifier);
            return flag;
        },
        [&](ArrayAccess &node) {
            return (checkExpressionForIdentifier(*node.array, identifier) || checkExpressionForIdentifier(*node.selector, identifier));
        },
        [&](ClassInstanceCreationExpression &node) {
            bool flag = false;
            for (auto &arg: node.arguments) flag = flag || checkExpressionForIdentifier(arg, identifier);
            return flag;
        },
        [&](InfixExpression &node) {
            return (checkExpressionForIdentifier(*node.expression1, identifier) || checkExpressionForIdentifier(*node.expression2, identifier));
        },
        [&](Assignment &node) {
            return (checkExpressionForIdentifier(*node.assigned_from, identifier) || checkExpressionForIdentifier(*node.assigned_to, identifier));
        },
        [&](QualifiedThis &node) { return false; },
        [&](auto &node) {
            return checkExpressionForIdentifier(*node.expression, identifier);
        }
    }, expr); 

    return false;
}