#include "cfg-builder.h"

void CfgBuilderVisitor::operator()(IfThenStatement &node) {

}

void CfgBuilderVisitor::operator()(IfThenElseStatement &node) {

}

void CfgBuilderVisitor::operator()(MethodDeclaration &node) {
    this->visit_children(node);

    // Set control flow graph for this method after visiting children   
    // Maybe this isn't needed, since we are already doing it in Block
}

void CfgBuilderVisitor::operator()(Block &node) {
    this->visit_children(node);

    // Set control flow graph for this block after visiting children
}