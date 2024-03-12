#include "cfg-visitor.h"
#include "exceptions/exceptions.h"
#include "variant-ast/classes.h"
#include <variant>

void CfgVisitor::operator()(MethodDeclaration &node) {
    operator()(node.cfg_start);
}

void CfgVisitor::visit_children(CfgStatement *node) {
    if ( node->next ) {
        operator()(node->next);
    }
}

void CfgVisitor::visit_children(CfgExpression *node) {
    if ( node->true_branch ) {
        operator()(node->true_branch);
    }
    if ( node->false_branch ) {
        operator()(node->false_branch);
    } 
}

void CfgVisitor::operator()(CfgNode *node) {
    CfgStatement* stmt = dynamic_cast<CfgStatement*>(node);
    if ( stmt != nullptr ) {
        this->visit_children(stmt);
    } else {
        CfgExpression* expr = dynamic_cast<CfgExpression*>(node);
        if ( expr != nullptr ) {
            this->visit_children(stmt);
        } else {
            THROW_CompilerError("Unable to convert CfgNode to Statement/Expression");
        }
    }
}

void CfgVisitor::operator()(CfgStatement *node) { this->visit_children(node); }
void CfgVisitor::operator()(CfgExpression *node) { this->visit_children(node); }
void CfgVisitor::visit(AstNodeVariant &node) { std::visit(*this, node); }
