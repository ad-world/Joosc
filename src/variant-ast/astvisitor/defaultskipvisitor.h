#pragma once

#include "variant-ast/astnode.h"
#include "astvisitor.h"
#include "variant-ast/expressions.h"

// Abstract visitor that by default, skips any node type an implementation is not overriden for and traverses the children
template <typename ReturnType>
class DefaultSkipVisitor : public AstVisitor<ReturnType> {
  public:
    using AstVisitor<ReturnType>::operator();
    virtual void operator()(CompilationUnit &node) override { this->visit_children(node); }

    virtual void operator()(QualifiedIdentifier &node) override { this->visit_children(node); }
    virtual void operator()(Identifier &node) override { this->visit_children(node); }

    virtual void operator()(Type &node) override { this->visit_children(node); }
    virtual void operator()(NonArrayType &node) override { this->visit_children(node); }
    virtual void operator()(PrimitiveType &node) override { this->visit_children(node); }

    virtual void operator()(ClassDeclaration &node) override { this->visit_children(node); }
    virtual void operator()(InterfaceDeclaration &node) override { this->visit_children(node); }
    virtual void operator()(FieldDeclaration &node) override { this->visit_children(node); }
    virtual void operator()(MethodDeclaration &node) override { this->visit_children(node); }
    virtual void operator()(VariableDeclarator &node) override { this->visit_children(node); }
    virtual void operator()(FormalParameter &node) override { this->visit_children(node); }
    virtual void operator()(Modifier &node) override { this->visit_children(node); }

    virtual void operator()(LocalVariableDeclaration &node) override { this->visit_children(node); }
    virtual void operator()(Block &node) override { this->visit_children(node); }
    virtual void operator()(IfThenStatement &node) override { this->visit_children(node); }
    virtual void operator()(IfThenElseStatement &node) override { this->visit_children(node); }
    virtual void operator()(WhileStatement &node) override { this->visit_children(node); }
    virtual void operator()(ForStatement &node) override { this->visit_children(node); }
    virtual void operator()(ReturnStatement &node) override { this->visit_children(node); }
    virtual void operator()(EmptyStatement &node) override { this->visit_children(node); }

    virtual void operator()(InfixExpression &node) override { this->visit_children(node); }
    virtual void operator()(PrefixExpression &node) override { this->visit_children(node); }
    virtual void operator()(CastExpression &node) override { this->visit_children(node); }
    virtual void operator()(Assignment &node) override { this->visit_children(node); }
    virtual void operator()(QualifiedThis &node) override { this->visit_children(node); }
    virtual void operator()(ArrayCreationExpression &node) override { this->visit_children(node); }
    virtual void operator()(Literal &node) override { this->visit_children(node); }
    virtual void operator()(ClassInstanceCreationExpression &node) override { this->visit_children(node); }
    virtual void operator()(FieldAccess &node) override { this->visit_children(node); }
    virtual void operator()(ArrayAccess &node) override { this->visit_children(node); }
    virtual void operator()(MethodInvocation &node) override { this->visit_children(node); }
    virtual void operator()(InstanceOfExpression &node) override { this->visit_children(node); }
    virtual void operator()(ParenthesizedExpression &node) override { this->visit_children(node); }
};
