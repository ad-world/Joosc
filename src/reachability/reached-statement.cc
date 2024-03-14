#include "reached-statement.h"
#include "exceptions/exceptions.h"
#include "reachability.h"
#include "utillities/overload.h"
#include "utillities/util.h"
#include "variant-ast/classes.h"
#include "variant-ast/statements.h"
#include <variant>

void checkStatement(Statement &stmt) {
    yy::location *location = nullptr;

    std::visit(util::overload{
        [&](IfThenStatement &node) -> void {
            location = &node.location;
            if ( node.then_clause ) {
                checkStatement(*node.then_clause);
            }
        },
        [&](IfThenElseStatement &node) -> void {
            location = &node.location;
            if ( node.then_clause ) {
                checkStatement(*node.then_clause);
            }
            if ( node.else_clause ) {
                checkStatement(*node.else_clause);
            }
        },
        [&](WhileStatement &node) -> void {
            location = &node.location;
            if ( node.body_statement ) {
                checkStatement(*node.body_statement);
            }
        },
        [&](ForStatement &node) -> void {
            location = &node.location;
            if ( node.body_statement ) {
                checkStatement(*node.body_statement);
            }
        },
        [&](Block &node) -> void {
            location = &node.location;
            for ( auto& stmt : node.statements ) {
                checkStatement(stmt);
            }
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

    // std::cout << &stmt << std::endl;
    if ( CfgReachabilityVisitor::reached.find(&stmt) == CfgReachabilityVisitor::reached.end() ) {
        THROW_ReachabilityError("Statement not reached: " + location_str);
    }
}

void StatementVisitor::operator()(MethodDeclaration &node) {
    std::string location;

    if ( node.body.get() == nullptr ) { return; }
    for ( auto &stmt : node.body->statements ) {
        checkStatement(stmt);
    }
}
