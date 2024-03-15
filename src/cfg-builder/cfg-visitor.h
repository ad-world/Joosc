#pragma once
#include "cfg-builder/cfg-builder.h"
#include <unordered_set>

class CfgVisitor : public DefaultSkipVisitor<void> {
protected:
    // Normally do not override
    void operator()(CfgNode *node);
    void visit_children(CfgStatement *stmt);
    void visit_children(CfgExpression *expr);

    MethodDeclarationObject* current_method;
    std::unordered_set<CfgNode*> visited;
    void visit_child(CfgNode* node);
public:
    // Ast visitor code
    using DefaultSkipVisitor<void>::operator();
    void operator()(MethodDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;
    void visit(AstNodeVariant &node) override;

    // Override these when extended
    virtual void operator()(CfgStatement *stmt);
    virtual void operator()(CfgExpression *expr);
};
