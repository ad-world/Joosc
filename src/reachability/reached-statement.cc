#include "reached-statement.h"
#include "exceptions/exceptions.h"
#include "reachability.h"
#include "utillities/overload.h"
#include "utillities/util.h"
#include "variant-ast/classes.h"
#include "variant-ast/statements.h"
#include <variant>

void checkStatement(Statement &stmt) {
    std::visit(util::overload{
        [&](IfThenStatement &node) -> void {
            if ( node.then_clause ) {
                checkStatement(*node.then_clause);
            }
        },
        [&](IfThenElseStatement &node) -> void {
            if ( node.then_clause ) {
                checkStatement(*node.then_clause);
            }
            if ( node.else_clause ) {
                checkStatement(*node.else_clause);
            }
        },
        [&](WhileStatement &node) -> void {
            if ( node.body_statement ) {
                checkStatement(*node.body_statement);
            }
        },
        [&](ForStatement &node) -> void {
            if ( node.body_statement ) {
                checkStatement(*node.body_statement);
            }
        },
        [&](Block &node) -> void {
            for ( auto& stmt : node.statements ) {
                checkStatement(stmt);
            }
        },
        [&](auto &node) -> void {}
     }, stmt);

    std::string location_str = Util::statementToLocationString(stmt);
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
