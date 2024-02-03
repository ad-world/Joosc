#pragma once

#include <vector>
#include "../../ast/ast.h"
#include <deque>
#include "../../parsing/bison/parser.hh"
#include <utility>

class Utils {
    private:

        std::vector<std::vector<AstNode*>> cachedClasses;
    public:
    std::string getParserType(int type);
    // Expects root to be CompilationUnit
    std::vector<AstNode*> getClasses(AstNode *root);
    // Expects *root to be ClassDeclaration
    std::vector<AstNode*> getFunctionsFromClass(AstNode *root);
    // Expects *root to be MethodDeclaration
    std::string getFunctionName(AstNode *function);
    // Expects *root to be ClassDeclaration
    std::string getClassName(AstNode *root);
    // Expects *root to be ClassDeclaration
    std::vector<std::string> getClassModifiers(AstNode *root);

    // Root can be anything
    std::vector<std::pair<AstNode *, AstNode *>> getLiteralPairs(AstNode *root);

    // Expects *root to be MethodDeclaration
    std::vector<std::string> getFunctionModifiers(AstNode *root);
    // Expects *root to be AstNode
    bool hasFunctionBody(AstNode *root);
};