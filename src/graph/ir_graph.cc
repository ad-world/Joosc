#include "ir_graph.h"

// CompUnitIR
void IRGraphVisitor::operator()(CompUnitIR &node) {
    addNode(&node, node.label());
    for ( auto &kv_pair : node.getFunctions() ) {
        addChild(&node, kv_pair.second);
    }
    this->visit_children(node);
}

// FuncDeclIR
void IRGraphVisitor::operator()(FuncDeclIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getBody()));
    this->visit_children(node);
}

// ExpressionIRs
void IRGraphVisitor::operator()(BinOpIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getLeft()));
    addChild(&node, getInnerAddr(node.getRight()));
    this->visit_children(node);
}
void IRGraphVisitor::operator()(CallIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getTarget()));
    for ( auto &arg : node.getArgs() ) {
        assert(arg);
        addChild(&node, getInnerAddr(*arg));
    }
    this->visit_children(node);
}
void IRGraphVisitor::operator()(ConstIR &node) {
    addNode(&node, node.label());
    this->visit_children(node);
}
void IRGraphVisitor::operator()(ESeqIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getStmt()));
    addChild(&node, getInnerAddr(node.getExpr()));
    this->visit_children(node);
}
void IRGraphVisitor::operator()(MemIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getAddress()));
    this->visit_children(node);
}
void IRGraphVisitor::operator()(NameIR &node) {
    addNode(&node, node.label());
    this->visit_children(node);
}
void IRGraphVisitor::operator()(TempIR &node) {
    addNode(&node, node.label());
    this->visit_children(node);
}

// StatementIRs
void IRGraphVisitor::operator()(CJumpIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getCondition()));
    this->visit_children(node);
}
void IRGraphVisitor::operator()(ExpIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getExpr()));
    this->visit_children(node);
}
void IRGraphVisitor::operator()(JumpIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getTarget()));
    this->visit_children(node);
}
void IRGraphVisitor::operator()(LabelIR &node) {
    addNode(&node, node.label());
    this->visit_children(node);
}
void IRGraphVisitor::operator()(MoveIR &node) {
    addNode(&node, node.label());
    addChild(&node, getInnerAddr(node.getTarget()));
    addChild(&node, getInnerAddr(node.getSource()));
    this->visit_children(node);
}
void IRGraphVisitor::operator()(ReturnIR &node) {
    addNode(&node, node.label());
    if ( node.getRet() ) {
        addChild(&node, getInnerAddr(*node.getRet()));
    }
    this->visit_children(node);
}
void IRGraphVisitor::operator()(SeqIR &node) {
    addNode(&node, node.label());
    for ( auto &stmt : node.getStmts() ) {
        assert(stmt);
        addChild(&node, getInnerAddr(*stmt));
    }
    this->visit_children(node);
}
