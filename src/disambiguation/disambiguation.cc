#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "exceptions/semanticerror.h"
#include "type-decl/type_declaration.h"

class DisambiguationVisitor : public DefaultSkipVisitor<void> {

public: 
    using DefaultSkipVisitor<void>::operator();

    void operator()(MethodInvocation &node) override;
    void operator()(ClassInstanceCreationExpression &node) override;
    void operator()(ArrayAccess &node) override;
    void operator()(Assignment &node) override;
    void operator()(LocalVariableDeclaration &node) override;
    void operator()(MethodDeclaration &node) override;
};