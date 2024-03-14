#include "cfg-builder/cfg-visitor.h"
#include <unordered_set>
#include <vector>

enum ReachabilityEnum {
    UNKNOWN, 
    POSSIBLE,
    IMPOSSIBLE
};


class CfgReachabilityVisitor : public CfgVisitor {
public:
    static std::unordered_set<Statement*> reached;
    
    std::vector<Statement*> statements;

    std::unordered_map<Statement*,  ReachabilityEnum> in;
    std::unordered_map<Statement*,  ReachabilityEnum> out;

    Statement* getPrevStatement();

    static bool isConstantExpression(Expression &node);
    static Literal evalConstantExpression(Expression &node);
public:
    void operator()(MethodDeclaration &node) override;
    void operator()(CfgStatement *stmt) override;
    void operator()(CfgExpression *expr) override;
};
