#pragma once

#include "IR/comp-unit/comp-unit.h"
#include "variant-ast/astnode.h"
#include "variant-ast/astvisitor/defaultskipvisitor.h"

class IRBuilderVisitor : public DefaultSkipVisitor<CompUnitIR> {
    CompUnitIR comp_unit;
    bool static_fields_only = false;

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
    using DefaultSkipVisitor<CompUnitIR>::operator();
    // void operator()(CompilationUnit &node) override;

    void operator()(ClassDeclaration &node) override;
    void operator()(MethodDeclaration &node) override;
    // void operator()(InterfaceDeclaration &node) override;
    // void operator()(FieldDeclaration &node) override;

    IRBuilderVisitor(bool static_fields_only = false) : comp_unit{""}, static_fields_only{static_fields_only} {}

    CompUnitIR visit(AstNodeVariant &ast) override {
        std::visit(*this, ast);
        return std::move(comp_unit);
    }
};
