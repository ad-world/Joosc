#include "cfg-builder.h"
#include "utillities/overload.h"
#include <iostream>
#include <vector>
#include <assert.h>
#include <vector>
#include "variant-ast/statements.h"
#include "variant-ast/expressions.h"

// We only want to start building CFGs from the MethodDeclaration node - our recursive function will handle the rest
void CfgBuilderVisitor::operator()(MethodDeclaration &node) {
    auto &method_block = node.body;

    if(!method_block) return;
    // Pair of CfgStatement* to represent the start and end nodes of the CFG
    std::vector<std::pair<CfgStatement*, CfgStatement*>> children;
    for (Statement &stmt : method_block->statements) {  
        auto pair = createCfg(stmt);
        children.push_back(pair);
    }

    if ( children.size() > 0 ) {
        // Linking the CFGs of the children together (pointing the next pointer of the last statement of one child to the first statement of the next child)
        for (int i = 0; i + 1 < children.size(); i++) {
            children[i].second->next = children[i + 1].first;
        }

        // Setting the start and end nodes of the top-levl CFG for the method declaration node
        if ( children.size() > 0 ) {
            node.cfg_start = children.front().first;
            node.cfg_end = children.back().second;
        }
    }
}

/*
    This function is responsible for building the CFG for a given statement. It is a recursive function that
    builds the CFG for a statement and all of its children. It returns a pair of CfgStatement* to represent the
    start and end nodes of the CFG for the statement.
*/
std::pair<CfgStatement*, CfgStatement*> CfgBuilderVisitor::createCfg(Statement &stmt) {
    CfgStatement *start = nullptr;
    CfgStatement *end = nullptr;

    std::visit(util::overload {
        [&](IfThenStatement &node) -> void {
            // Recursively build CFG for the then clause
            auto child_statement = createCfg(*node.then_clause);

            // Create start and end dummy nodes
            auto dummy_start = new CfgStatement(true, false);
            auto dummy_end = new CfgStatement(false, true);
            
            // Create a CFG Expression based on if clause
            auto expr = new CfgExpression(node.if_clause.get());
            expr->source_statement = &stmt;

            // Point start to the expression
            dummy_start->next = expr;
            // Point the true branch of the expression to the start of the then clause
            expr->true_branch = child_statement.first;
            // Point the false branch of the expression to the end of the dummy end node
            expr->false_branch = dummy_end;
            // Point the end of the then clause to the dummy end node
            child_statement.second->next = dummy_end;

            start = dummy_start;
            end = dummy_end;
        },
        [&](IfThenElseStatement &node) -> void {
            // Recursively build CFG for the then and else clauses
            auto then_statement = createCfg(*node.then_clause);
            auto else_statement = createCfg(*node.else_clause);

            auto dummy_start = new CfgStatement(true, false);
            auto dummy_end = new CfgStatement(false, true);

            // Create a CFG Expression based on if clause
            auto expr = new CfgExpression(node.if_clause.get());
            expr->source_statement = &stmt;
            // Point the start to the expression
            dummy_start->next = expr;
            // Point the true branch of the expression to the start of the then clause
            expr->true_branch = then_statement.first;
            // Point the false branch of the expression to the start of the else clause
            expr->false_branch = else_statement.first;
            // Point the end of the then and else clauses to the dummy end node
            then_statement.second->next = dummy_end;
            else_statement.second->next = dummy_end;

            start = dummy_start;
            end = dummy_end;
        },
        [&](WhileStatement &node) -> void{
            // Recursively build CFG for the body of the while statement
            auto body = createCfg(*node.body_statement);
            auto dummy_start = new CfgStatement(true, false);
            auto dummy_end = new CfgStatement(false, true);

            // Create a CFG Expression based on while clause
            auto expr = new CfgExpression(node.condition_expression.get());
            expr->source_statement = &stmt;
            expr->is_for_while = true;
            // Point the start to the expression
            dummy_start->next = expr;
            // Point the true branch of the expression to the start of the while body
            expr->true_branch = body.first;
            // Point the false branch of the expression to the end of the dummy end node
            expr->false_branch = dummy_end;
            // Point the end of the while body to the expression
            body.second->next = expr;

            start = dummy_start;
            end = dummy_end;
        },
        [&](ForStatement &node) -> void{
            // Recursively build CFG for the body of the for statement
            auto body = createCfg(*node.body_statement);
            auto dummy_start = new CfgStatement(true, false);
            auto dummy_end = new CfgStatement(false, true);
            
            // Create a CFG Expression based on for clause condition
            auto expr = new CfgExpression(node.condition_expression.get());
            expr->source_statement = &stmt;
            expr->is_for_while = true;
            // Point the start to the expression
            dummy_start->next = expr;
            // Point the true branch of the expression to the start of the for body
            expr->true_branch = body.first;
            // Point the false branch of the expression to the dummy end node
            expr->false_branch = dummy_end;
            // Point the end of the for body back to the expression
            body.second->next = expr;

            start = dummy_start;
            end = dummy_end;
        },
        [&](ReturnStatement &node) -> void {
            // Create a CFG Statement based on return statement
            auto return_statement = new CfgStatement(&stmt, true); // Modify the uctor of CfgStatement to accept a const ReturnStatement& parameter
            start = return_statement;
            end = return_statement;
        },
        [&](Block &node) -> void {
            // Recursively build CFG for the block, building a CFG for each statement in the block
            std::vector<std::pair<CfgStatement*, CfgStatement*>> children;
            for (Statement &new_stmt : node.statements) {
                children.push_back(createCfg(new_stmt));
            }

            if ( children.size() > 0 ) {
                // Linking statements together
                for (int i = 0; i + 1 < children.size(); i++) {
                    children[i].second->next = children[i + 1].first;
                }

                auto dummy_start = new CfgStatement(&stmt);
                dummy_start->is_starting_node = true;
                dummy_start->next = children.front().first;

                start = dummy_start;
                end = children.back().second;
            } else {
                auto dummy_stmt = new CfgStatement(&stmt);
                start = dummy_stmt;
                end = dummy_stmt;
            }
        },
        [&](ExpressionStatement &node) -> void {
            // Create a CFG Statement based on expression statement
            // All three cases are the same, so we can have a default visitor for each node type
            std::visit(util::overload {
                [&](auto &node) -> void {
                    auto empty_statement = new CfgStatement(&stmt);
                    start = empty_statement;
                    end = empty_statement;
                }
            }, node);
        },
        // Default case for all other statement types
        [&](auto &node) -> void {
            auto empty_statement = new CfgStatement(&stmt);
            start = empty_statement;
            end = empty_statement;
        },
     }, stmt);

    assert(start != nullptr);
    assert(end != nullptr);

    return std::make_pair(start, end);
}

CfgExpression::CfgExpression(Expression* expression) : expression(expression) {}
CfgStatement::CfgStatement() : is_return(false) {}
CfgStatement::CfgStatement(bool is_start, bool is_end) : in(is_start), is_starting_node{is_start}, is_ending_node{is_end} {}
CfgStatement::CfgStatement(Statement* statement) : statement(statement) {}
CfgStatement::CfgStatement(Statement* statement, bool is_return): statement(statement), is_return(is_return) {}
CfgBuilderVisitor::CfgBuilderVisitor() {}
