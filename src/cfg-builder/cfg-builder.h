#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include <variant>
#include <vector>
#include "statements.h"

/*
This visitor is responsible for building the control flow graph (CFG) of our program / a method.

The CFG is a directed graph that represents the flow of control in a program. It is a useful tool for
analyzing the control flow of a program, and is used in many compiler optimizations and static analysis
tools. The nodes of the graph represent basic blocks, which are sequences of instructions with a single
entry point and a single exit point. The edges of the graph represent the flow of control between basic
blocks. The CFG is used to analyze the control flow of a program, and to perform optimizations such as
dead code elimination, loop unrolling, and loop-invariant code motion.


*/
class CfgBuilderVisitor : public DefaultSkipVisitor<void> {

public:
    CfgBuilderVisitor() {}

    using DefaultSkipVisitor<void>::operator();
    void operator()(IfThenStatement &node) override;
    void operator()(IfThenElseStatement &node) override;
    void operator()(MethodDeclaration &node) override;
    void operator()(Block &node) override;

};