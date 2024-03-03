#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "environment-builder/symboltable.h"

class InterfaceExtender : DefaultSkipVisitor<void> {
    PackageDeclarationObject* root_package;
public:
    // Should never be called on their own in this class, but must be public for std::visit
    using DefaultSkipVisitor<void>::operator();
    void operator()(InterfaceDeclaration &node) override;

    InterfaceExtender(PackageDeclarationObject &default_package)
        : root_package(&default_package) {};

    void visit(AstNodeVariant &node) override;
};
