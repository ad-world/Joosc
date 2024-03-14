#pragma once

#include <vector>
#include <string>
#include <variant>
#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "variant-ast/expressions.h"
#include "type-decl/linkedtype.h"

class TypeChecker: public DefaultSkipVisitor<void> {

    PackageDeclarationObject* default_package;
    CompilationUnitNamespace compilation_unit_namespace;
    MethodDeclarationObject* current_method;
    ClassDeclarationObject* current_class;

    // Shorthand for getting linked type from any expression node
    LinkedType getLink(std::unique_ptr<Expression>& node_ptr);
    ClassDeclarationObject* getStringClass(LinkedType &link);

    // Helper methods
    bool checkifMethodIsAccessible(
      MethodDeclarationObject* method_to_access, LinkedType type_method_called_on
    );

  public:
    using DefaultSkipVisitor<void>::operator();

    static LinkedType getLink(Expression &node);
    // Operations for finding the types of identifiers
    void operator()(CompilationUnit &node) override;

    void operator()(ClassDeclaration &node) override;

    void operator()(MethodDeclaration &node) override;

    void operator()(Block &node) override;

    void operator()(LocalVariableDeclaration &node) override;

    void operator()(QualifiedIdentifier &node) override;

    // All subexpression types
    void operator()(Assignment &node) override; //

    void operator()(QualifiedThis &node) override;

    void operator()(ArrayCreationExpression &node) override;

    void operator()(ClassInstanceCreationExpression &node) override;

    void operator()(FieldAccess &node) override; //

    void operator()(ArrayAccess &node) override;

    void operator()(MethodInvocation &node) override; //

    void operator()(InfixExpression &node) override;

    void operator()(PrefixExpression &node) override;

    void operator()(CastExpression &node) override;

    void operator()(InstanceOfExpression &node) override;

    void operator()(ParenthesizedExpression &node) override;

    TypeChecker(PackageDeclarationObject& default_package);

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
