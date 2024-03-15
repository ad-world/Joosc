#include "cfg-builder/cfg-visitor.h"
#include <unordered_map>

class DeadAssignmentVisitor : public CfgVisitor {
    void handleAssignment(Assignment &assignment);
    void checkExpressionForUsedAssignments(Expression &expr);
    void checkStatementForUsedAssignments(Statement &stmt);
public:
    std::unordered_map<std::string, Assignment*> unused_assignment;
    void operator()(CfgStatement *stmt) override;
    void operator()(CfgExpression *expr) override;
};
