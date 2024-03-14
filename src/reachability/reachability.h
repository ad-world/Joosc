#include "cfg-builder/cfg-visitor.h"

class CfgReachabilityVisitor : public CfgVisitor {
    bool isString() { return false; } // TODO: temp function - remove

public:
    static bool isConstantExpression(Expression &node);
    static Literal evalConstantExpression(Expression &node);
public:
    void operator()(CfgStatement *stmt) override;
    void operator()(CfgExpression *expr) override;
};
