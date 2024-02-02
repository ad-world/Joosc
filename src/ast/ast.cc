#include "ast.h"

AstNode::AstNode() {
}

AstNode::AstNode(int type, std::string value) {
    this->type = type;
    this->value = value;
}

AstNode::AstNode(int type) {
    this->type = type;
};

void AstNode::addChild(AstNode *node) {
    node->setParent(this);
    children.push_back(node);
};

void AstNode::setParent(AstNode* parent) {
    this->parent = parent;
}
