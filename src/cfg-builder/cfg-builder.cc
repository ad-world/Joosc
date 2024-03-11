#include "cfg-builder.h"

void CfgBuilderVisitor::operator()(IfThenStatement &node) {

}

void CfgBuilderVisitor::operator()(IfThenElseStatement &node) {

}

void CfgBuilderVisitor::operator()(WhileStatement &node) {

}

void CfgBuilderVisitor::operator()(ForStatement &node) {

}

void CfgBuilderVisitor::operator()(MethodDeclaration &node) {
    this->visit_children(node);

    // Check that all execution paths have a return statement at the end
}

void CfgBuilderVisitor::operator()(Block &node) {
    CfgBlock* block = new CfgBlock();
    block->block = &node;
    block_stack.push(block);
    this->visit_children(node);

    node.cfg_block = block_stack.top();
    block_stack.pop();
    // Set control flow graph for this block after visiting children
}