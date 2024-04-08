#pragma once

#include "IR/ir_visitor.h"
#include "IR/ir.h"
#include "string.h"
#include <vector>

class IRJavaConverter : public IRSkipVisitor {
    // A list of function names that have been encountered in this specific CompUnit
    std::vector<std::string> functions;

    // A variable storing the outputted Java code
    std::string result;

    // The current class name
    std::string class_name;

    // Number of tabs to produce on current line when outputted to result
    int num_tabs;

    // Get a string of num_tabs concatenated
    std::string getTabString();

    // Get a string of \n and num_tabs concatenated
    std::string getNewlineAndTabString();

    // Append a specific line of code to the result
    void appendToResult(std::string s);

public:
    using IRSkipVisitor::visit;

    IRJavaConverter(std::string class_name);

    std::string getResult() { return result; }

    // Overriding visitor operators
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