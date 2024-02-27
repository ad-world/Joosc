#pragma once

#include <vector>
#include <string>
#include <variant>
#include "variant-ast/astvisitor/defaultskipvisitor.h"

class TypeChecker: public DefaultSkipVisitor<bool> {
public:
    using DefaultSkipVisitor<bool>::operator();
    void operator()(CompilationUnit &node) override;
    void operator()(ClassDeclaration &node) override;
    void operator()(MethodDeclaration &node) override;
    void operator()(FieldDeclaration &node) override;
    void operator()(FormalParameter &node) override;
    void operator()(LocalVariableDeclaration &node) override;
    void operator()(ReturnStatement &node) override;
    void operator()(Assignment &node) override;
    void operator()(InfixExpression &node) override;
    void operator()(CastExpression &node) override;
};