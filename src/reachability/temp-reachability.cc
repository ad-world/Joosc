#include "temp-reachability.h"
#include "exceptions/exceptions.h"

void ReachabilityVisitor::operator()(MethodDeclaration &node) {
    auto cfg_start = node.cfg_start;
    if ( !cfg_start ) { return; }

    annotateReachable(cfg_start, node);
    visited.clear();
    checkReachability(cfg_start, node);
}

void ReachabilityVisitor::annotateReachable(CfgNode *node, MethodDeclaration &method) {
    if ( !node ) { return; }

    if (visited.find(node) == visited.end()) {
        visited.insert(node);
    } else {
        return;
    }

    if ( CfgStatement* stmt = dynamic_cast<CfgStatement*>(node) ) {
        if (stmt->is_return) {
            stmt->out = false;
        } else {
            stmt->out = stmt->in;
        }

        if (stmt->next) {
            if (auto st = dynamic_cast<CfgStatement*>(stmt->next)) {
                st->in = st->in || stmt->out;
            } 
        }
        
        annotateReachable(stmt->next, method);
    } else if ( CfgExpression* expr = dynamic_cast<CfgExpression*>(node) ) {
        annotateReachable(expr->true_branch, method);
        annotateReachable(expr->false_branch, method);
    } else {
        THROW_CompilerError("Unable to convert CfgNode to Statement/Expression");
    }
};

void ReachabilityVisitor::checkReachability(CfgNode* node, MethodDeclaration &method) {
    if ( !node ) { return; }

    if (visited.find(node) == visited.end()) {
        visited.insert(node);
    } else {
        return;
    }

    if ( CfgStatement* stmt = dynamic_cast<CfgStatement*>(node) ) {
        if (!stmt->in && stmt->next == nullptr && !stmt->is_return) {
            std::cout << stmt->is_return << std::endl;
            THROW_ReachabilityError("Unreachable code found");
        }

        checkReachability(stmt->next, method);
    } else if ( CfgExpression* expr = dynamic_cast<CfgExpression*>(node) ) {
        checkReachability(expr->true_branch, method);
        checkReachability(expr->false_branch, method);
    } else {
        THROW_CompilerError("Unable to convert CfgNode to Statement/Expression");
    }
};