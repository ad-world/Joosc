#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "exceptions/exceptions.h"

class SearchUnclassifiedVisitor : public DefaultSkipVisitor<void> {

public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(Identifier &node) override {
        if ( node.classification == Classification::UNCLASSIFIED ) {
            THROW_CompilerError("We have an unclassified identifier: " + node.name);
        }
    }

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
