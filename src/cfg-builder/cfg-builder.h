#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include <variant>
#include <vector>
#include "variant-ast/statements.h"
#include <stack>

/*
    This visitor is responsible for building the control flow graph (CFG) of our program / a method.

    The CFG is a directed graph that represents the flow of control in a program. It is a useful tool for
    analyzing the control flow of a program, and is used in many compiler optimizations and static analysis
    tools. The nodes of the graph represent basic blocks, which are sequences of instructions with a single
    entry point and a single exit point. The edges of the graph represent the flow of control between basic
    blocks. The CFG is used to analyze the control flow of a program, and to perform optimizations such as
    dead code elimination, loop unrolling, and loop-invariant code motion.
*/

class CfgNode {};

struct CfgBlock : public CfgNode {
    Block* block;
    std::vector<CfgBlock*> successors;
    std::vector<CfgBlock*> predecessors;
};

struct CfgIf : public CfgNode {
    CfgNode* parent;
    IfThenStatement* if_stmt;
    CfgBlock* then_block;
};

struct CfgMethod : public CfgNode {
    std::vector<CfgBlock*> blocks;
    CfgBlock* entry_block;
    CfgBlock* exit_block;
};

struct CfgIfThenElse : public CfgNode {
    IfThenElseStatement* if_else_stmt;
    Statement* then_block;
    Statement* else_block;
};

struct CfgWhile : public CfgNode {
    std::unique_ptr<Expression> condition;
    CfgBlock* body_block;
};

struct CfgFor : public CfgNode {
    Statement* init;
    Expression* condition;
    Statement* update;
    CfgBlock* body_block;
};

struct CfgReturn : public CfgNode {
    std::unique_ptr<Expression> return_value;
};

class CfgBuilderVisitor : public DefaultSkipVisitor<void> {
    std::stack<CfgBlock*> block_stack;
public:
    CfgBuilderVisitor() {}

    using DefaultSkipVisitor<void>::operator();
    void operator()(IfThenStatement &node) override;
    void operator()(IfThenElseStatement &node) override;
    void operator()(WhileStatement &node) override;
    void operator()(ForStatement &node) override;
    void operator()(MethodDeclaration &node) override;
    void operator()(ReturnStatement &node) override;
    void operator()(ExpressionStatement &node) override;
    void operator()(Statement &node) override;
    void operator()(Block &node) override;

};