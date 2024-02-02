#include "utils.h"


#define GET_NAME(symbol) \
    (((symbol) == yy::parser::symbol_kind::S_YYEMPTY) ? "YYEMPTY" \
        : yy::parser::symbol_name( \
            static_cast<yy::parser::symbol_kind_type>((symbol))))

std::string Utils::getParserType(int type) {
    return GET_NAME(type);
}

// Expects root to be CompilationUnit
std::vector<AstNode*> Utils::getClasses(AstNode* root) {
    if(cachedClasses.size() > 0) {
        return cachedClasses.at(0);
    }

    std::vector<AstNode*> res;
    deque<AstNode *> q;
    q.push_back(root);

    while (!q.empty()) {
        AstNode* curr = q.front();

        q.pop_front();
        
        if (getParserType(curr->type) == "ClassDeclaration") {
            res.push_back(curr);
        } else {
            for ( auto child : curr->children ) {
                q.push_back(child);
            } 
        }
    }

    cachedClasses.push_back({ res });

    return res;
}

// Expects *root to be ClassDeclaration
std::vector<AstNode *> Utils::getFunctionsFromClass(AstNode* root) {
    std::vector<AstNode*> res;
    deque<AstNode *> q;
    q.push_back(root);

    while(!q.empty()) {
        AstNode *curr = q.front();
        q.pop_front();

        if (getParserType(curr->type) == "MethodDeclaration") {
            res.push_back(curr);
        } else {
            for (auto child : curr->children ) {
                q.push_back(child);
            } 
        }
    }

    return res;
} 

// Expects *function to be MethodDeclaration
std::string Utils::getFunctionName(AstNode *function) {
    AstNode* methodHeader = function->children[0];

    for (const auto& node: methodHeader->children) {
        if(getParserType(node->type) == "MethodDeclarator") {
            std::string ident = get<string>(node->children[0]->value.value());

            return ident;
        }
    }

    return "";
}

// Expects *root to be ClassDeclaration
std::string Utils::getClassName(AstNode *root) {
    for (const auto& node: root->children) {
        if (getParserType(node->type) == "IDENTIFIER") {
            return get<string>(node->value.value());
        } 
    }

    return "";
}

std::vector<std::string> Utils::getClassModifiers(AstNode *root) {
    std::vector<std::string> result;

    if (getParserType(root->children[0]->type) == "Modifiers") {
        deque<AstNode* > q;
        q.push_front(root->children[0]);

        while(!q.empty()) {
            AstNode* top = q.front();
            q.pop_front();

            if(top->children.size() == 0) {
                result.push_back(getParserType(top->type));
            } else {
                for (auto child: top->children) {
                    q.push_back(child);
                }
            }
        }
    }
    
    return result;
}

