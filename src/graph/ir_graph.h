#pragma once

#include "IR/ir_visitor.h"
#include "utillities/overload.h"
#include <fstream>
#include <sstream>

class IRGraphVisitor : public IRSkipVisitor {
    std::ostringstream buffer;

    void* getInnerAddr(IR &node) {
        return std::visit(util::overload {
            [&](StatementIR &innernode) { return (void*) getInnerAddr(innernode); },
            [&](ExpressionIR &innernode) { return (void*) getInnerAddr(innernode); },
            [&](auto &innernode) {
                return (void*) &innernode;
            }
        }, node);
    }
    void* getInnerAddr(StatementIR &node) {
        return std::visit([&](auto &innernode) {
            return (void*) &innernode;
        }, node);
    }
    void* getInnerAddr(ExpressionIR &node) {
        return std::visit([&](auto &innernode) {
            return (void*) &innernode;
        }, node);
    }

    void addNode(void* address, std::string label) {
        buffer << "\"" << address << "\"";
        buffer << " [label=\"" << label << "\"]";
        buffer << "\n";
    }
    void addChild(void* parent_addr, void* child_addr) {
        buffer << "\"" << parent_addr << "\"";
        buffer << " -> ";
        buffer << "\"" << child_addr << "\"";
        buffer << "\n";
    }
    void addChild(void* parent_addr, IR &child_node) { addChild(parent_addr, getInnerAddr(child_node)); }
    void addChild(void* parent_addr, ExpressionIR &child_node) { addChild(parent_addr, getInnerAddr(child_node)); }
    void addChild(void* parent_addr, StatementIR &child_node) { addChild(parent_addr, getInnerAddr(child_node)); }

public:
    ~IRGraphVisitor() {
        std::ofstream outfile;
        outfile.open("graphs/ir_graph.gv");
        outfile << "digraph G {\n";
        outfile << buffer.str();
        outfile << "}\n";
        outfile.close();
    }

    // CompUnitIR
    virtual void operator()(CompUnitIR &node) override;

    // FuncDeclIR
    virtual void operator()(FuncDeclIR &node) override;

    // ExpressionIRs
    virtual void operator()(BinOpIR &node) override;
    virtual void operator()(CallIR &node) override;
    virtual void operator()(ConstIR &node) override;
    virtual void operator()(ESeqIR &node) override;
    virtual void operator()(MemIR &node) override;
    virtual void operator()(NameIR &node) override;
    virtual void operator()(TempIR &node) override;

    // StatementIRs
    virtual void operator()(CJumpIR &node) override;
    virtual void operator()(ExpIR &node) override;
    virtual void operator()(JumpIR &node) override;
    virtual void operator()(LabelIR &node) override;
    virtual void operator()(MoveIR &node) override;
    virtual void operator()(ReturnIR &node) override;
    virtual void operator()(SeqIR &node) override;
};
