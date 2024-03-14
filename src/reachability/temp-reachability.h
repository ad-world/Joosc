
#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "cfg-builder/cfg-builder.h"

class ReachabilityVisitor : public DefaultSkipVisitor<void> {
    void annotateReachable(CfgNode *node, MethodDeclaration &method);
    void checkReachability(CfgNode *node, MethodDeclaration &method);
    std::unordered_set<CfgNode*> visited;
public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(MethodDeclaration &node) override;

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};