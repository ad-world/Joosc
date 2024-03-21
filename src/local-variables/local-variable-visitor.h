#pragma once
#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "environment-builder/symboltableentry.h"

class LocalVariableVisitor : public DefaultSkipVisitor<void> {
    MethodDeclarationObject* current_method;
    void checkExpressionForIdentifier(Expression &expr, Identifier &identifier);
    LinkedType getLink(Expression &node);
public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(MethodDeclaration &node) override;
    void operator()(LocalVariableDeclaration &node) override;

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
