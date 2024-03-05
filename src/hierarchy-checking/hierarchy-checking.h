#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "environment-builder/symboltableentry.h"

class HierarchyCheckingVisitor : public DefaultSkipVisitor<void> {
    PackageDeclarationObject* root_symbol_table;
    
public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(ClassDeclaration &node) override;

    void operator()(InterfaceDeclaration &node) override;

    void visit(AstNodeVariant &node) override;

    HierarchyCheckingVisitor(PackageDeclarationObject &root_symbol_table);
};
