#pragma once

#include <vector>
#include "../../ast/ast.h"
#include <deque>
#include "../../parsing/bison/parser.hh"

class Utils {
    private:

        std::vector<std::vector<AstNode*>> cachedClasses;
        std::string getParserType(int type);
    public:
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

};
