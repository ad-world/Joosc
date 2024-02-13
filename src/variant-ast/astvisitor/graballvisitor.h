#pragma once

#include "variant-ast/astnode.h"
#include "defaultskipvisitor.h"

// Visitor that grabs all nodes to the tree of a specific node type
// Pointers will be invalidated if original tree goes out of scope
template <typename GrabType>
class GrabAllVisitor : public DefaultSkipVisitor<std::vector<GrabType*>> {
    std::vector<GrabType*> result;

  public:
    using DefaultSkipVisitor<std::vector<GrabType*>>::operator();
    void operator()(GrabType &node) override {
        result.push_back(&node);
        this->visit_children(node);
    }

    std::vector<GrabType*> visit(AstNodeVariant &node) override {
        std::visit(*this, node);
        return result;
    }
};
