#include "dead-assignment.h"
#include "utillities/overload.h"
#include "utillities/util.h"
#include "variant-ast/names.h"
#include "variant-ast/statements.h"
#include "exceptions/exceptions.h"
#include <cassert>
#include <variant>

bool DeadAssignmentVisitor::checkWarnings(std::vector<AstNodeVariant> &nodes) {
    for (auto &node : nodes) {
        this->visit(node);
    }

    checkAssignments();

    if (warnings.size() > 0) {
        for (auto &warning : warnings) {
            std::cerr << "Warning: " << warning << std::endl;
        }
        return true;
    }

    return false;
}

void DeadAssignmentVisitor::checkAssignments() {
    if (unused_assignment.size() > 0) {
        for (auto &unused : unused_assignment) {
            std::string location_str = Util::locationToString(unused.second.first->location);
            warnings.push_back("Local variable " + unused.first + " is assigned to but never used at: " + location_str);
        }
    }
}

void DeadAssignmentVisitor::handleAssignment(Assignment &assignment) {
    assert(assignment.assigned_to != nullptr);
    std::visit(util::overload{
        [&](QualifiedIdentifier &qi) {
            if ( qi.identifiers.size() == 1 ) {
                // We have a local variable assignment (TODO: could this be a field assignment?)
                auto identifier = qi.identifiers.back();
                // std::cout << "Assigning to " << identifier.name << " at " << Util::locationToString(qi.location) << std::endl;
                if ( unused_assignment.find(identifier.name) != unused_assignment.end() ) {
                    auto prev_assignment = unused_assignment.at(identifier.name);
                    // Previous assignment is unused before redfining in the same scope - throw a warning
                    if (prev_assignment.second == current_scope_id) {
                        std::string location_str = Util::locationToString(prev_assignment.first->location);
                        warnings.push_back("Local variable " + identifier.name + " is assigned to but never used at: " + location_str);
                    }
                }
                // Set this assignment as unused
                unused_assignment[identifier.name] = { &assignment, current_scope_id };;
            }
        },
        [&](auto &node) {}
    }, *assignment.assigned_to);
}

void DeadAssignmentVisitor::checkExpressionForUsedAssignments(Expression &expr) {
    std::visit(util::overload{
        [&](QualifiedIdentifier &qi) {
            // if ( qi.identifiers.size() == 1 ) {
                // We have a local variable assignment (TODO: could this be a field assignment?)
                auto identifier = qi.identifiers.back();
                // Only erase if the usage is not in the same statement as the assignment
                // i.e a = new JwDeadAssigment_2().a 

                auto assignment = unused_assignment.find(identifier.name);

                if ( assignment != unused_assignment.end() && !Util::checkExpressionForIdentifier(*assignment->second.first->assigned_from, identifier)) {
                    // std::cout << "Usage of " << qi.identifiers.back().name << " at " << Util::locationToString(qi.location) << std::endl;   
                    unused_assignment.erase(identifier.name);

                    // std::cout << "Erasing " << identifier.name << " from unused assignments" << std::endl;
                    if ( unused_assignment.find(identifier.name) != unused_assignment.end() ) {
                        // std::cout << identifier.name << " still in unused assignments" << std::endl;
                    } 
                }
            // }
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
        [&](FieldAccess &node) {
            if ( node.expression ) {
                checkExpressionForUsedAssignments(*node.expression);
            }
        },
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
            checkExpressionForUsedAssignments(*node.if_clause);
            checkStatementForUsedAssignments(*node.then_clause);
        },
        [&](IfThenElseStatement &node) {
            checkExpressionForUsedAssignments(*node.if_clause);
            checkStatementForUsedAssignments(*node.then_clause);
            checkStatementForUsedAssignments(*node.else_clause);
        },
        [&](WhileStatement &node) {
            checkExpressionForUsedAssignments(*node.condition_expression);
            checkStatementForUsedAssignments(*node.body_statement);
        },
        [&](ForStatement &node) { 
            checkStatementForUsedAssignments(*node.init_statement);
            checkExpressionForUsedAssignments(*node.condition_expression);
            checkStatementForUsedAssignments(*node.update_statement);
            checkStatementForUsedAssignments(*node.body_statement);
        },
        [&](Block &node) {
            current_scope_id = node.scope_id;
            for (auto &stmt : node.statements) {
                checkStatementForUsedAssignments(stmt);
            }
        },
        [&](EmptyStatement &node) {},
        [&](ExpressionStatement &node) {
            if (std::holds_alternative<Assignment>(node)) {
                auto &assignment = std::get<Assignment>(node);
                // handleAssignment(assignment);
                if (assignment.assigned_from) {
                    checkExpressionForUsedAssignments(*assignment.assigned_from);
                }
            } else if (std::holds_alternative<MethodInvocation>(node)) {
                auto &method = std::get<MethodInvocation>(node);
                for (auto &arg : method.arguments) {
                    checkExpressionForUsedAssignments(arg);
                }
            } else if (std::holds_alternative<ClassInstanceCreationExpression>(node)) {
                auto &cls = std::get<ClassInstanceCreationExpression>(node);
                for (auto &arg : cls.arguments) {
                    checkExpressionForUsedAssignments(arg);
                }
            }
        },
        [&](ReturnStatement &node) {
            if ( node.return_expression ) {
                checkExpressionForUsedAssignments(*node.return_expression);
            }
        },
        [&](LocalVariableDeclaration &node) {
            // do nothing
            if ( node.variable_declarator->expression) {
                checkExpressionForUsedAssignments(*node.variable_declarator->expression);
            }
        },
    }, stmt);
}

void DeadAssignmentVisitor::operator()(CfgStatement *stmt) {
    if (current_method->is_constructor) return;

    if ( stmt->statement != nullptr ) {
        // std::visit(util::overload{
        //     [&](ExpressionStatement &node) {
        //         std::visit(util::overload{
        //             [&](Assignment &assignment) {
        //                 handleAssignment(assignment);
        //             },
        //             [&](auto &node) {}
        //         }, node);
        //     },
        //     [&](auto &node) {}
        // }, *stmt->statement);

        checkStatementForUsedAssignments(*stmt->statement);
    }

    this->visit_children(stmt);
}

void DeadAssignmentVisitor::operator()(CfgExpression *expr) {
    if (current_method->is_constructor) return;

    if (expr->expression != nullptr) {
        checkExpressionForUsedAssignments(*expr->expression);
    }

    this->visit_children(expr);
}
