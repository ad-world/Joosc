#include "cfg-builder.h"
#include "utillities/overload.h"
#include <vector>
#include <assert.h>
#include <vector>
#include "variant-ast/statements.h"
#include <stack>
#include "variant-ast/expressions.h"
#include "type-decl/type_declaration.h"
#include "type-decl/linkedtype.h"


void CfgBuilderVisitor::operator()(MethodDeclaration &node) {
    auto &method_block = node.body;

    if(!method_block) return;

    std::vector<std::pair<CfgStatement*, CfgStatement*>> children;
    for (Statement &stmt : method_block->statements) {  
        auto pair = createCfg(stmt);
        children.push_back(pair);
    }

    // [0]: while statement
    // [1]: return statement
    for (int i = 0; i < children.size() - 1; i++) {
        children[i].second->next = children[i + 1].first;
    }

    node.cfg_start = children.front().first;
    node.cfg_end = children.back().second;
}

std::pair<CfgStatement*, CfgStatement*> CfgBuilderVisitor::createCfg(Statement &stmt) {
    CfgStatement *start = nullptr;
    CfgStatement *end = nullptr;
    // build CFG based on stmt

    std::visit(util::overload {
        [&](IfThenStatement &node) -> void {
            // build CFG for if-then statement
            auto child_statement = createCfg(*node.then_clause);

            auto dummy_start = new CfgStatement();
            auto dummy_end = new CfgStatement();
            
            auto expr = new CfgExpression(node.if_clause.get());
            dummy_start->next = expr;
            expr->true_branch = child_statement.first;
            expr->false_branch = dummy_end;
            child_statement.second->next = dummy_end;

            start = dummy_start;
            end = dummy_end;
        },
        [&](IfThenElseStatement &node) -> void {
            auto then_statement = createCfg(*node.then_clause);
            auto else_statement = createCfg(*node.else_clause);

            // build CFG for if-then-else statement
            auto dummy_start = new CfgStatement();
            auto dummy_end = new CfgStatement();

            auto expr = new CfgExpression(node.if_clause.get());
            dummy_start->next = expr;
            expr->true_branch = then_statement.first;
            expr->false_branch = else_statement.first;
            then_statement.second->next = dummy_end;
            else_statement.second->next = dummy_end;

            start = dummy_start;
            end = dummy_end;
        },
        [&](WhileStatement &node) -> void{
            auto body = createCfg(*node.body_statement);
            auto dummy_start = new CfgStatement();
            auto dummy_end = new CfgStatement();

            // build CFG for while statement
            auto expr = new CfgExpression(node.condition_expression.get());
            dummy_start->next = expr;
            expr->true_branch = body.first;
            expr->false_branch = dummy_end;
            body.second->next = expr;

            start = dummy_start;
            end = dummy_end;
        },
        [&](ForStatement &node) -> void{
            auto body = createCfg(*node.body_statement);
            auto dummy_start = new CfgStatement();
            auto dummy_end = new CfgStatement();
            
            // build CFG for for statement
            auto expr = new CfgExpression(node.condition_expression.get());
            dummy_start->next = expr;
            expr->true_branch = body.first;
            expr->false_branch = dummy_end;
            body.second->next = expr;

            start = dummy_start;
            end = dummy_end;
        },
        [&](ReturnStatement &node) -> void {
            auto return_statement = new CfgStatement(&stmt, true); // Modify the uctor of CfgStatement to accept a const ReturnStatement& parameter
            start = return_statement;
            end = return_statement;
        },
        [&](Block &node) -> void {
            // build CFG for block
            std::vector<std::pair<CfgStatement*, CfgStatement*>> children;
            for (Statement &new_stmt : node.statements) {
                children.push_back(createCfg(new_stmt));
            }

            for (int i = 0; i < children.size() - 1; i++) {
                children[i].second->next = children[i + 1].first;
            }

            start = children.front().first;
            end = children.back().second;
        },
        [&](ExpressionStatement &node) -> void {
            // build CFG for expression statement
            std::visit(util::overload {
                [&](auto &node) -> void {
                    auto empty_statement = new CfgStatement(&stmt);
                    start = empty_statement;
                    end = empty_statement;
                }
            }, node);
        },
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