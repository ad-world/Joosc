#pragma once

#include "IR/ir_visitor.h"
#include "IR/ir.h"

class IRJavaConverter : public IRSkipVisitor {
    
public:
    using IRSkipVisitor::visit;
    void operator()(BinOpIR &node) override;
};