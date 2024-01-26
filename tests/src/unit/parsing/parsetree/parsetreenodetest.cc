#include "parsing/parsetree/parsetreenode.h"
#include "parsing/parsetree/parsetreenode_t.h"
#include <vector>
#include <gtest/gtest.h>

// ParseTreeNodes can be constructed without semantic values
TEST(ParseTreeNodeTest, NoSemanticConstructor) {
    auto node = ParseTreeNode(ParseTreeNode_t::CompilationUnit);
}

// ParseTreeNodes can be constructed with semantic values
TEST(ParseTreeNodeTest, SemanticConstructor) {
    auto node1 = ParseTreeNode(ParseTreeNode_t::Block, 10);
    auto node2 = ParseTreeNode(ParseTreeNode_t::Primary, "foo");
    auto node3 = ParseTreeNode(ParseTreeNode_t::InterfaceMethodDeclaratorRest, std::vector<int>());
}

// addChild can be used to add children to a ParseTreeNode
TEST(ParseTreeNodeTest, addChild) {
    auto node0 = ParseTreeNode(ParseTreeNode_t::CompilationUnit);
    auto node1 = ParseTreeNode(ParseTreeNode_t::Block, 10);
    auto node2 = ParseTreeNode(ParseTreeNode_t::Primary, "foo");
    auto node3 = ParseTreeNode(ParseTreeNode_t::InterfaceMethodDeclaratorRest, std::vector<int>());
    node2.addChild(node0);
    node2.addChild(node1);
    node3.addChild(node2);
}
