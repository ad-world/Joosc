#pragma once

#include "ir.h"

class IRSkipVisitor {
protected:
    // CompUnitIR
    void visit_children(CompUnitIR &node);

    // FuncDeclIR
    void visit_children(FuncDeclIR &node);

    // ExpressionIRs
    void visit_children(std::unique_ptr<ExpressionIR> &node);
    void visit_children(ExpressionIR &node);
    void visit_children(BinOpIR &node);
    void visit_children(CallIR &node);
    void visit_children(ConstIR &node);
    void visit_children(ESeqIR &node);
    void visit_children(MemIR &node);
    void visit_children(NameIR &node);
    void visit_children(TempIR &node);

    // StatementIRs
    void visit_children(std::unique_ptr<StatementIR> &node);
    void visit_children(StatementIR &node);
    void visit_children(CJumpIR &node);
    void visit_children(ExpIR &node);
    void visit_children(JumpIR &node);
    void visit_children(LabelIR &node);
    void visit_children(MoveIR &node);
    void visit_children(ReturnIR &node);
    void visit_children(SeqIR &node);

public:
    // CompUnitIR
    virtual void operator()(CompUnitIR &node) { this->visit_children(node); }

    // FuncDeclIR
    virtual void operator()(FuncDeclIR &node) { this->visit_children(node); }

    // ExpressionIRs
    virtual void operator()(std::unique_ptr<ExpressionIR> &node) { this->visit_children(node); }
    virtual void operator()(ExpressionIR &node) { this->visit_children(node); }
    virtual void operator()(BinOpIR &node) { this->visit_children(node); }
    virtual void operator()(CallIR &node) { this->visit_children(node); }
    virtual void operator()(ConstIR &node) { this->visit_children(node); }
    virtual void operator()(ESeqIR &node) { this->visit_children(node); }
    virtual void operator()(MemIR &node) { this->visit_children(node); }
    virtual void operator()(NameIR &node) { this->visit_children(node); }
    virtual void operator()(TempIR &node) { this->visit_children(node); }

    // StatementIRs
    virtual void operator()(std::unique_ptr<StatementIR> &node) { this->visit_children(node); }
    virtual void operator()(StatementIR &node) { this->visit_children(node); }
    virtual void operator()(CJumpIR &node) { this->visit_children(node); }
    virtual void operator()(ExpIR &node) { this->visit_children(node); }
    virtual void operator()(JumpIR &node) { this->visit_children(node); }
    virtual void operator()(LabelIR &node) { this->visit_children(node); }
    virtual void operator()(MoveIR &node) { this->visit_children(node); }
    virtual void operator()(ReturnIR &node) { this->visit_children(node); }
    virtual void operator()(SeqIR &node) { this->visit_children(node); }

    // Can call either visitor(node) or visitor.visit(node) as the entrypoint
    void operator()(IR &node) {
        return visit(node);
    }

    virtual void visit(IR &node) { std::visit(*this, node); };
};
