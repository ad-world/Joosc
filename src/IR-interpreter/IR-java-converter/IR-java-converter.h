#pragma once

#include "IR/ir_visitor.h"
#include "IR/ir.h"
#include "string.h"
#include <vector>

class IRJavaConverter : public IRSkipVisitor {
    std::vector<std::string> functions;
    std::string result;
    std::string class_name;
    int num_tabs;

    std::string getTabString();
    std::string getNewlineAndTabString();
    std::string getFunctionName(std::string functionName);
    void appendToResult(std::string s);

public:
    using IRSkipVisitor::visit;

    IRJavaConverter(std::string class_name);

    std::string getResult() { return result; }

    void operator()(CompUnitIR &node) override;
    void operator()(FuncDeclIR &node) override;

    void operator()(BinOpIR &node) override;
    void operator()(CallIR &node) override;
    void operator()(ConstIR &node) override;
    void operator()(ESeqIR &node) override;
    void operator()(MemIR &node) override;
    void operator()(NameIR &node) override;
    void operator()(TempIR &node) override;

    void operator()(CJumpIR &node) override;
    void operator()(ExpIR &node) override;
    void operator()(JumpIR &node) override;
    void operator()(LabelIR &node) override;
    void operator()(MoveIR &node) override;
    void operator()(ReturnIR &node) override;
    void operator()(SeqIR &node) override;
};