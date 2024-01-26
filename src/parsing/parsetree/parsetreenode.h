#pragma once
#include <memory>
#include <vector>
#include "parsetreenode_t.h"

class SemanticValue{};

template <typename T> struct Value : public SemanticValue {
  Value(T value) : value{value} {}
  T value;
};

struct ParseTreeNode {
  ParseTreeNode() {}

  // Construct Parse Tree Node with no semantic value
  ParseTreeNode(ParseTreeNode_t type) : 
    type{type}, semantic_value{nullptr}, children{} {}

  // Construct Parse Tree Node with semantic value
  template <typename T> ParseTreeNode(ParseTreeNode_t type, T value) : 
    type{type}, semantic_value{std::make_unique<Value>(value)}, children{} {}

  ParseTreeNode_t type;
  std::unique_ptr<SemanticValue> semantic_value;
  std::vector<ParseTreeNode> children;
};
