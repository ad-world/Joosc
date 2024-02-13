#pragma once

#include "variant-ast/astnode.h"
#include <variant>

template <typename ReturnType>
class AstVisitor {
  protected:
    // Call visit on each child node of the ast node
    void visit_children(CompilationUnit &node);

    void visit_children(QualifiedIdentifier &node);
    void visit_children(Identifier &node);

    void visit_children(Type &node);
    void visit_children(NonArrayType &node);
    void visit_children(PrimitiveType &node);

    void visit_children(ClassDeclaration &node);
    void visit_children(InterfaceDeclaration &node);
    void visit_children(FieldDeclaration &node);
    void visit_children(MethodDeclaration &node);
    void visit_children(VariableDeclarator &node);
    void visit_children(FormalParameter &node);
    void visit_children(Modifier &node);

    void visit_children(LocalVariableDeclaration &node);
    void visit_children(Block &node);
    void visit_children(IfThenStatement &node);
    void visit_children(IfThenElseStatement &node);
    void visit_children(WhileStatement &node);
    void visit_children(ForStatement &node);
    void visit_children(ReturnStatement &node);
    void visit_children(EmptyStatement &node);

    void visit_children(InfixExpression &node);
    void visit_children(PrefixExpression &node);
    void visit_children(CastExpression &node);
    void visit_children(Assignment &node);
    void visit_children(QualifiedThis &node);
    void visit_children(ArrayCreationExpression &node);
    void visit_children(Literal &node);
    void visit_children(ClassInstanceCreationExpression &node);
    void visit_children(FieldAccess &node);
    void visit_children(ArrayAccess &node);
    void visit_children(MethodInvocation &node);

    void visit_children(Statement &node);
    void visit_children(Expression &node);
    void visit_children(ExpressionStatement &node);

  public:
    virtual ~AstVisitor() = default;

    virtual void operator()(CompilationUnit &node) = 0;

    virtual void operator()(QualifiedIdentifier &node) = 0;
    virtual void operator()(Identifier &node) = 0;

    virtual void operator()(Type &node) = 0;
    virtual void operator()(NonArrayType &node) = 0;
    virtual void operator()(PrimitiveType &node) = 0;

    virtual void operator()(ClassDeclaration &node) = 0;
    virtual void operator()(InterfaceDeclaration &node) = 0;
    virtual void operator()(FieldDeclaration &node) = 0;
    virtual void operator()(MethodDeclaration &node) = 0;
    virtual void operator()(VariableDeclarator &node) = 0;
    virtual void operator()(FormalParameter &node) = 0;
    virtual void operator()(Modifier &node) = 0;

    virtual void operator()(LocalVariableDeclaration &node) = 0;
    virtual void operator()(Block &node) = 0;
    virtual void operator()(IfThenStatement &node) = 0;
    virtual void operator()(IfThenElseStatement &node) = 0;
    virtual void operator()(WhileStatement &node) = 0;
    virtual void operator()(ForStatement &node) = 0;
    virtual void operator()(ReturnStatement &node) = 0;
    virtual void operator()(EmptyStatement &node) = 0;

    virtual void operator()(InfixExpression &node) = 0;
    virtual void operator()(PrefixExpression &node) = 0;
    virtual void operator()(CastExpression &node) = 0;
    virtual void operator()(Assignment &node) = 0;
    virtual void operator()(QualifiedThis &node) = 0;
    virtual void operator()(ArrayCreationExpression &node) = 0;
    virtual void operator()(Literal &node) = 0;
    virtual void operator()(ClassInstanceCreationExpression &node) = 0;
    virtual void operator()(FieldAccess &node) = 0;
    virtual void operator()(ArrayAccess &node) = 0;
    virtual void operator()(MethodInvocation &node) = 0;

    virtual void operator()(Statement &node);
    virtual void operator()(Expression &node);
    virtual void operator()(ExpressionStatement &node);

    // Can call either visitor(node) or visitor.visit(node) as the entrypoint
    ReturnType operator()(AstNodeVariant &node) {
        return visit(node);
    }

    virtual ReturnType visit(AstNodeVariant &node) = 0;
};

#include "astvisitor_impl.h"
