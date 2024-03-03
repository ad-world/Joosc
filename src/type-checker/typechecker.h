#pragma once

#include <vector>
#include <string>
#include <variant>
#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "variant-ast/expressions.h"
#include "type-decl/linkedtype.h"

class TypeChecker: public DefaultSkipVisitor<bool> {

    // Shorthand for getting linked type from any expression node
    LinkedType getLink(Expression &node) {
        return std::visit([](auto expr_type) -> LinkedType {
            return expr_type.link;
        }, node);
    }
    LinkedType getLink(std::unique_ptr<Expression>& node_ptr) {
        return getLink(*node_ptr);
    }

  public:
    using DefaultSkipVisitor<bool>::operator();

    // All subexpression types
    void operator()(Assignment &node) override;

    void operator()(QualifiedThis &node) override;

    void operator()(ArrayCreationExpression &node) override;

    void operator()(ClassInstanceCreationExpression &node) override;

    void operator()(FieldAccess &node) override;

    void operator()(ArrayAccess &node) override;

    void operator()(MethodInvocation &node) override;

    void operator()(InfixExpression &node) override;

    void operator()(PrefixExpression &node) override;

    void operator()(CastExpression &node) override;

    void operator()(InstanceOfExpression &node) override;
};