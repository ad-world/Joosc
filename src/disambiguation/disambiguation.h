#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "variant-ast/expressions.h"
#include "type-decl/type_declaration.h"
#include "type-decl/linkedtype.h"
#include "environment-builder/symboltableentry.h"
#include <variant>
#include <vector>

/*
A name is syntactically classified as an ExpressionName in these contexts:
    As the qualifying expression in a qualified class instance creation expression (§15.9)
    As the array reference expression in an array access expression (§15.13)
    As the left-hand operand of an assignment operator (§15.26)

A name is syntactically classified as a MethodName in this context:
    Before the "(" in a method invocation expression (§15.12)

A name is syntactically classified as an AmbiguousName in these contexts:
    To the left of the "." in a qualified ExpressionName
    To the left of the "." in a qualified MethodName
    To the left of the "." in a qualified AmbiguousName
*/

class DisambiguationVisitor : public DefaultSkipVisitor<void> {
    PackageDeclarationObject *default_package = nullptr;
    PackageDeclarationObject *current_package = nullptr;
    MethodDeclarationObject *current_method = nullptr;
    ClassDeclarationObject *current_class = nullptr;
    InterfaceDeclarationObject *current_interface = nullptr;
    CompilationUnit *compilation_unit = nullptr;

    void disambiguate(QualifiedIdentifier &qi);
    void checkForwardDeclaration(const std::string usage, std::string potential_forward_dec);
    // void getQualifiedIdentifersFromExpression(const Expression &expr, std::vector<QualifiedIdentifier>& identifiers);

public: 
    DisambiguationVisitor(PackageDeclarationObject &default_package): default_package(&default_package), current_package(&default_package) {}

    using DefaultSkipVisitor<void>::operator();
    void operator()(MethodInvocation &node) override;
    void operator()(ClassInstanceCreationExpression &node) override;
    void operator()(ArrayAccess &node) override;
    void operator()(Assignment &node) override;
    void operator()(MethodDeclaration &node) override;
    void operator()(FieldAccess &node) override;
    void operator()(ClassDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;
    void operator()(CompilationUnit &node) override;
    void operator()(FieldDeclaration &node) override;
    void operator()(CastExpression &node) override;
    void operator()(ParenthesizedExpression &node) override;

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
