#include "cfg-builder/cfg-builder.h"

class CfgVisitor {
protected:
    // Normally do not override
    void operator()(CfgNode *node);
    void visit_children(CfgStatement *stmt);
    void visit_children(CfgExpression *expr);
public:
    // Override these
    virtual void operator()(CfgStatement *stmt);
    virtual void operator()(CfgExpression *expr);

    void visit(CfgStatement *start_node);
};
