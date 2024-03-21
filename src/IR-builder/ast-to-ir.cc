#include "ast-to-ir.h"
#include "IR/comp-unit/comp-unit.h"
#include "IR/func-decl/func-decl.h"

void IRBuilderVisitor::operator()(ClassDeclaration &node) {
    // TODO: implement
    
    // CREATE CompUnit
    CompUnitIR comp_unit = {node.environment->identifier};
}

void IRBuilderVisitor::operator()(MethodDeclaration &node) {
    // TODO: implement

    // Convert CFG to IR

    // CREATE FuncDecl
    FuncDeclIR func_decl = {
        node.environment->identifier,
        nullptr,                        // TODO: add body
        (int) node.parameters.size()
    };
}

// Rest of the operators are probably not needed?
