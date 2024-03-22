#pragma once

#include "IR/comp-unit/comp-unit.h"
#include "variant-ast/astnode.h"
#include "variant-ast/astvisitor/defaultskipvisitor.h"

class IRBuilderVisitor : public DefaultSkipVisitor<void> {
    std::unique_ptr<CompUnitIR> comp_unit;

    // Statement converters
    std::unique_ptr<StatementIR> convert(Statement &stmt);
    std::unique_ptr<StatementIR> convert(IfThenStatement &stmt);
    std::unique_ptr<StatementIR> convert(IfThenElseStatement &stmt);
    std::unique_ptr<StatementIR> convert(WhileStatement &stmt);
    std::unique_ptr<StatementIR> convert(ForStatement &stmt);
    std::unique_ptr<StatementIR> convert(Block &stmt);
    std::unique_ptr<StatementIR> convert(EmptyStatement &stmt);
    std::unique_ptr<StatementIR> convert(ExpressionStatement &stmt);
    std::unique_ptr<StatementIR> convert(ReturnStatement &stmt);
    std::unique_ptr<StatementIR> convert(LocalVariableDeclaration &stmt);

    // Expression converters
    std::unique_ptr<ExpressionIR> convert(Expression &expr);
    std::unique_ptr<ExpressionIR> convert(Assignment &expr);
    std::unique_ptr<ExpressionIR> convert(InfixExpression &expr);
    std::unique_ptr<ExpressionIR> convert(PrefixExpression &expr);
    std::unique_ptr<ExpressionIR> convert(CastExpression &expr);
    std::unique_ptr<ExpressionIR> convert(Literal &expr);
    std::unique_ptr<ExpressionIR> convert(ClassInstanceCreationExpression &expr);
    std::unique_ptr<ExpressionIR> convert(FieldAccess &expr);
    std::unique_ptr<ExpressionIR> convert(MethodInvocation &expr);
    std::unique_ptr<ExpressionIR> convert(ArrayAccess &expr);
    std::unique_ptr<ExpressionIR> convert(QualifiedThis &expr);
    std::unique_ptr<ExpressionIR> convert(ArrayCreationExpression &expr);
    std::unique_ptr<ExpressionIR> convert(QualifiedIdentifier &expr);
    std::unique_ptr<ExpressionIR> convert(InstanceOfExpression &expr);
    std::unique_ptr<ExpressionIR> convert(ParenthesizedExpression &expr);

public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(CompilationUnit &node) override;

    void operator()(QualifiedIdentifier &node) override;
    void operator()(Identifier &node) override;

    void operator()(Type &node) override;
    void operator()(NonArrayType &node) override;
    void operator()(PrimitiveType &node) override;

    void operator()(ClassDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;
    void operator()(FieldDeclaration &node) override;
    void operator()(MethodDeclaration &node) override;
    void operator()(VariableDeclarator &node) override;
    void operator()(FormalParameter &node) override;
    void operator()(Modifier &node) override;

    void operator()(LocalVariableDeclaration &node) override;
    void operator()(Block &node) override;
    void operator()(IfThenStatement &node) override;
    void operator()(IfThenElseStatement &node) override;
    void operator()(WhileStatement &node) override;
    void operator()(ForStatement &node) override;
    void operator()(ReturnStatement &node) override;
    void operator()(EmptyStatement &node) override;

    void operator()(InfixExpression &node) override;
    void operator()(PrefixExpression &node) override;
    void operator()(CastExpression &node) override;
    void operator()(Assignment &node) override;
    void operator()(QualifiedThis &node) override;
    void operator()(ArrayCreationExpression &node) override;
    void operator()(Literal &node) override;
    void operator()(ClassInstanceCreationExpression &node) override;
    void operator()(FieldAccess &node) override;
    void operator()(ArrayAccess &node) override;
    void operator()(MethodInvocation &node) override;
    void operator()(InstanceOfExpression &node) override;
    void operator()(ParenthesizedExpression &node) override;

    void visit(AstNodeVariant &ast) override {
        std::visit(*this, ast);
    }
};
