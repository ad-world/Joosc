#pragma once
#include <variant>
#include <vector>
#include <optional>
#include <string>

class AstNode;

// #include "parsing/bison/parser.hh"

using namespace std;

class AstNode {
public:
    AstNode* parent = NULL;
    vector<AstNode *> children;
    int type;
    std::string value;
    AstNode();
    AstNode(int type);
    AstNode(int type, std::string value);
    void addChild(AstNode *node);
    void setParent(AstNode *parent);
};