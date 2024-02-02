#pragma once
#include <variant>
#include <vector>
#include <optional>
#include <string>

class AstNode;

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
    template<typename... Args>
    void addChild(AstNode* first, Args ...rest) {
        children.push_back(first);
        addChild(rest...);
    }
    void setParent(AstNode *parent);
};