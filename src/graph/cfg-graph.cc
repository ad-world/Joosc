#include "cfg-graph.h"
#include "utillities/util.h"
#include <variant>

std::string StatementToLabel(Statement &stmt) {
    std::string label;
    std::visit(util::overload{
        [&](IfThenStatement &node) { label = "IfThenStatement"; },
        [&](IfThenElseStatement &node) { label = "IfThenElseStatement"; },
        [&](WhileStatement &node) { label = "WhileStatement"; },
        [&](ForStatement &node) { label = "ForStatement"; },
        [&](Block &node) { label = "Block"; },
        [&](EmptyStatement &node) { label = "EmptyStatement"; },
        [&](ExpressionStatement &node) {
            std::visit(util::overload{
                [&](Assignment &node) { label = "Assigment"; },
                [&](MethodInvocation &node) { label = "MethodInvocation"; },
                [&](ClassInstanceCreationExpression &node) { label = "ClassInstanceCreationExpression"; },
            }, node);
        },
        [&](ReturnStatement &node) { label = "ReturnStatement"; },
        [&](LocalVariableDeclaration &node) { label = "LocalVariableDeclaration"; },
    }, stmt);

    label += "\\n" + Util::statementToLocationString(stmt);

    return label;
}

void CfgGraph::operator()(CfgStatement *stmt) {
    std::string label = "STATEMENT";
    if ( stmt->statement ) {
        label += "\\n" + StatementToLabel(*stmt->statement);
    } else {
        label += "\\nDUMMY";
    }

    // Label the node
    graph << "\"" << stmt << "\"";
    graph << " [label=\"" << label << "\"]\n";

    // Connect the node to children
    if ( stmt->next ) {
        graph << "\"" << stmt << "\"";
        graph << " -> ";
        graph << "\"" << stmt->next << "\"";
        graph << "\n";
    }

    this->visit_children(stmt);
}

void CfgGraph::operator()(CfgExpression *expr) {
    std::string label = "EXPRESSION";
    if ( expr->source_statement ) {
        label += "\n" + StatementToLabel(*expr->source_statement);
    } else {
        label += "\nDUMMY";
    }

    // Label the node
    graph << "\"" << expr << "\"";
    graph << " [label=\"" << label << "\"]\n";

    // Connect the node to children

    // True branch
    if ( expr->true_branch ) {
        graph << "\"" << expr << "\"";
        graph << " -> ";
        graph << "\"" << expr->true_branch << "\"";
        graph << " " << " [color=\"green\"]\n";
    }

    // False branch
    if ( expr->false_branch ) {
        graph << "\"" << expr << "\"";
        graph << " -> ";
        graph << "\"" << expr->false_branch << "\"";
        graph << " " << " [color=\"red\"]\n";
    }

    this->visit_children(expr);
}
