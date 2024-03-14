#include "cfg-builder/cfg-visitor.h"
#include <unordered_set>

class CfgReachabilityVisitor : public CfgVisitor {
public:
    static std::unordered_set<Statement*> reached;
    static bool isConstantExpression(Expression &node);
    static Literal evalConstantExpression(Expression &node);
public:
    void operator()(CfgStatement *stmt) override;
    void operator()(CfgExpression *expr) override;
};
