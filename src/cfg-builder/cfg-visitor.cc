#include "cfg-visitor.h"
#include "exceptions/exceptions.h"
#include "variant-ast/classes.h"
#include <variant>

void CfgVisitor::operator()(MethodDeclaration &node) {
    visit_child(node.cfg_start);
}

void CfgVisitor::visit_child(CfgNode* child) {
    if ( child && (visited.find(child) == visited.end()) ) {
        // Not visited before
        visited.insert(child);
        operator()(child);
    }
}

void CfgVisitor::visit_children(CfgStatement *node) {
    if ( !node ) { return; }

    visit_child(node->next);
}

void CfgVisitor::visit_children(CfgExpression *node) {
    if ( !node ) { return; }

    visit_child(node->true_branch);
    visit_child(node->false_branch);
}

void CfgVisitor::operator()(CfgNode *node) {
    if ( !node ) { return; }

    if ( CfgStatement* stmt = dynamic_cast<CfgStatement*>(node) ) {
        this->visit_children(stmt);
    } else if ( CfgExpression* expr = dynamic_cast<CfgExpression*>(node) ) {
        this->visit_children(expr);
    } else {
        THROW_CompilerError("Unable to convert CfgNode to Statement/Expression");
    }
}

void CfgVisitor::operator()(CfgStatement *node) { this->visit_children(node); }
void CfgVisitor::operator()(CfgExpression *node) { this->visit_children(node); }
void CfgVisitor::visit(AstNodeVariant &node) { std::visit(*this, node); }
