#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include <variant>


/*
    This visitor is responsible for building the control flow graph (CFG) of our program / a method.

    The CFG is a directed graph that represents the flow of control in a program. It is a useful tool for
    analyzing the control flow of a program, and is used in many compiler optimizations and static analysis
    tools. The nodes of the graph represent basic blocks, which are sequences of instructions with a single
    entry point and a single exit point. The edges of the graph represent the flow of control between basic
    blocks. The CFG is used to analyze the control flow of a program, and to perform optimizations such as
    dead code elimination, loop unrolling, and loop-invariant code motion.
*/

// Unused base class
class CfgNode {
public:
    virtual ~CfgNode() {}
};

// Used for branches in the CFG (eg. ifthen, while)
struct CfgExpression : public CfgNode {
    Statement* source_statement = nullptr;
    Expression* expression = nullptr;

    CfgNode* true_branch = nullptr;
    CfgNode* false_branch = nullptr;

    CfgExpression(Expression* expression);
};

// Used for a straight path through the CFG (eg. assignment, start/end)
struct CfgStatement : public CfgNode {
    Statement* statement = nullptr;
    bool is_return = false;
    CfgNode* next = nullptr;

    bool is_starting_node = false;
    bool is_ending_node = false;

    bool in = false;
    bool out = false;

    CfgStatement();
    CfgStatement(bool is_start, bool is_end);
    CfgStatement(Statement* statement);
    CfgStatement(Statement* statement, bool is_return);
};


class CfgBuilderVisitor : public DefaultSkipVisitor<void> {
    std::pair<CfgStatement*, CfgStatement*> createCfg(Statement &stmt);
public:
    CfgBuilderVisitor();

    using DefaultSkipVisitor<void>::operator();
    void operator()(MethodDeclaration &node) override;

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
