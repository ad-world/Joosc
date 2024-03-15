#include "dead-assignment.h"
#include "utillities/overload.h"
#include "utillities/util.h"
#include "variant-ast/names.h"
#include "variant-ast/statements.h"
#include "exceptions/exceptions.h"
#include <cassert>
#include <variant>

void DeadAssignmentVisitor::handleAssignment(Assignment &assignment) {
    assert(assignment.assigned_to != nullptr);
    std::visit(util::overload{
        [&](QualifiedIdentifier &qi) {
            if ( qi.identifiers.size() == 1 ) {
                // We have a local variable assignment (TODO: could this be a field assignment?)
                auto identifier = qi.identifiers.back();
                if ( unused_assignment.find(identifier.name) != unused_assignment.end() ) {
                    // Previous assignment is unused - throw a warning
                    std::string location_str = Util::locationToString(unused_assignment.at(identifier.name)->location);
                    THROW_DeadAssignmentWarning("Warning: Unused assignment " + location_str);
                }
                // Set this assignment as unused
                unused_assignment[identifier.name] = &assignment;
            }
        },
        [&](auto &node) {}
    }, *assignment.assigned_to);
}

void DeadAssignmentVisitor::checkExpressionForUsedAssignments(Expression &expr) {
    std::visit(util::overload{
        [&](QualifiedIdentifier &qi) {
            if ( qi.identifiers.size() == 1 ) {
                // We have a local variable assignment (TODO: could this be a field assignment?)
                auto identifier = qi.identifiers.back();
                unused_assignment.erase(identifier.name);
            }
        },
        [&](Assignment &node) {
            handleAssignment(node);
            if ( node.assigned_from ) {
                checkExpressionForUsedAssignments(*node.assigned_from);
            }
        },
        [&](InfixExpression &node) {
            if ( node.expression1 ) {
                checkExpressionForUsedAssignments(*node.expression1);
            }
            if ( node.expression2 ) {
                checkExpressionForUsedAssignments(*node.expression2);
            }
        },
        [&](ArrayAccess &node) {
            if ( node.array ) {
                checkExpressionForUsedAssignments(*node.array);
            }
            if ( node.selector ) {
                checkExpressionForUsedAssignments(*node.selector);
            }
        },
        [&](ClassInstanceCreationExpression &node) {
            for ( auto &arg : node.arguments ) {
                checkExpressionForUsedAssignments(arg);
            }
        },
        [&](MethodInvocation &node) {
            for ( auto &arg : node.arguments ) {
                checkExpressionForUsedAssignments(arg);
            }
        },
        [&](QualifiedThis &node) { /* pass */ },
        [&](Literal &node) { /* pass */ },
        // [&](ParenthesizedExpression &node) {},
        // [&](InstanceOfExpression &node) {},
        // [&](ArrayCreationExpression &node) {},
        // [&](FieldAccess &node) {},
        // [&](CastExpression &node) {},
        // [&](PrefixExpression &node) {},
        [&](auto &node) {
            if ( node.expression ) {
                checkExpressionForUsedAssignments(*node.expression);
            }
        }
    }, expr);
}

void DeadAssignmentVisitor::checkStatementForUsedAssignments(Statement &stmt) {
    std::visit(util::overload{
        [&](IfThenStatement &node) {

        },
        [&](IfThenElseStatement &node) {

        },
        [&](WhileStatement &node) {

        },
        [&](ForStatement &node) {

        },
        [&](Block &node) {

        },
        [&](EmptyStatement &node) {

        },
        [&](ExpressionStatement &node) {

        },
        [&](ReturnStatement &node) {

        },
        [&](LocalVariableDeclaration &node) {

        },
    }, stmt);
}

void DeadAssignmentVisitor::operator()(CfgStatement *stmt) {
    if ( stmt->statement != nullptr ) {
        std::visit(util::overload{
            [&](ExpressionStatement &node) {
                std::visit(util::overload{
                    [&](Assignment &assignment) {
                        handleAssignment(assignment);
                    },
                    [&](auto &node) {}
                }, node);
            },
            [&](auto &node) {}
        }, *stmt->statement);

        checkStatementForUsedAssignments(*stmt->statement);
    }

    this->visit_children(stmt);
}

void DeadAssignmentVisitor::operator()(CfgExpression *expr) {
    this->visit_children(expr);
}
