#pragma once

#include <iostream>
#include <vector>
#include "../ast/ast.h"
#include "utils/utils.h"

class Weeder {
public:
    // TODO: send root of parse tree to this function
    int weed(AstNode* root, std::string fileName);

    Weeder();

private:
    std::vector<std::string> violations;
    Utils *util;

    void checkAsciiRange(const std::string& source);
    
    // TODO: send vector of class nodes to this function. It will check modifiers + constructors
    // It will also check that constructors do not include explicit this() or super()
    void checkClassModifiersAndConstructors(std::vector<AstNode*> classes, std::string filename);

    void checkInterfaces(std::vector<AstNode*> classes, std::string filename); 
    
    // TODO: send vector class of methods to this function. This function will check if there is body if it is not native or abstract.
    // It will also check there are no explicit this() or super() calls if there is a function body
    void checkMethodModifiersAndBody(std::vector<AstNode*> methods);

    // TODO: figure out how this function will work 
    void checkClassNameWithFile();

    // Checks cast expressions are valid by ensuring all Expression refers to QualifiedIdentifier if it should
    void checkCastExpressionsValid(AstNode *root);

    // Add a violation to the vector
    void addViolation(const std::string& vio);

    // Print a list of violations, if found
    void printViolations();

    // Get filename
    std::string getFilename(std::string& filePath);

    bool isClassPublic(AstNode* root);

    // Get all literals and ensure they are correct
    void checkLiterals(AstNode *root);

    // Check fields are not final
    void checkClassFields(std::vector<AstNode*> fields);
};
