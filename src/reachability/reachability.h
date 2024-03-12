#include "cfg-builder/cfg-visitor.h"

class ReachabilityVisitor : public CfgVisitor {
    bool isString() { return false; } // TODO: temp function - remove

    bool isConstantExpression(Expression &node);
    bool evalConstantExpression(Expression &node);
public:
    void operator()(CfgStatement *stmt) override;
    void operator()(CfgExpression *expr) override;

    void visit(CfgStatement *start_node); // leave default
};
