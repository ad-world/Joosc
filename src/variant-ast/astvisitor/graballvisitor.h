#pragma once

#include "variant-ast/astnode.h"
#include "defaultskipvisitor.h"

// Visitor that grabs all nodes to the tree of a specific node type
// Pointers will be invalidated if original tree goes out of scope
template <typename GrabType>
class GrabAllVisitor : public DefaultSkipVisitor<std::vector<GrabType*>> {
  public:
    std::vector<GrabType*> result;

    using DefaultSkipVisitor<std::vector<GrabType*>>::operator();
    void operator()(GrabType &node) override {
        result.push_back(&node);
        this->visit_children(node);
    }
    
    // Visit using AstNodeVariant member
    template <typename NodeType>
    std::vector<GrabType*> visit(NodeType &node) {
        this->operator()(node);
        return result;
    }

    std::vector<GrabType*> visit(AstNodeVariant &node) override {
        std::visit(*this, node);
        return result;
    }
};
