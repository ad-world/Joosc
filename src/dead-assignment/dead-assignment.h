#pragma once

#include "cfg-builder/cfg-visitor.h"
#include <unordered_map>
#include <vector>
#include <string>

class DeadAssignmentVisitor : public CfgVisitor {
    void handleAssignment(Assignment &assignment);
    void checkExpressionForUsedAssignments(Expression &expr);
    void checkStatementForUsedAssignments(Statement &stmt);
    std::vector<std::string> warnings;
    void checkAssignments(); 
    int current_scope_id = -1;
public:
    std::unordered_map<std::string, std::pair<Assignment*, int>> unused_assignment;
    void operator()(CfgStatement *stmt) override;
    void operator()(CfgExpression *expr) override;
    bool checkWarnings(std::vector<AstNodeVariant> &nodes);
};
