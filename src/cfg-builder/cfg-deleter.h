#pragma once
#include "cfg-visitor.h"

class CfgDeleter : public CfgVisitor {
public:
    void operator()(CfgStatement *stmt) override {
        this->visit_children(stmt);
        delete stmt;
    }
    void operator()(CfgExpression *expr) override {
        this->visit_children(expr);
        delete expr;
    }
};
