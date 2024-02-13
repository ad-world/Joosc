#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <variant>
#include "../variant-ast/astnode.h"

using namespace std;
class AstWeeder {
public: 
    int weed(CompilationUnit root, string fileName);

    AstWeeder();

private:
    vector<string> violations;

    void checkAsciiRange(const string& source);

    void checkClassModifiersAndConstructors(vector<ClassDeclaration>, string filename);

    void checkInterfaces(vector<InterfaceDeclaration>, string filename);

    void checkMethodModifiersAndBody(vector<MethodDeclaration> methods);

    void checkCastExpressionsValid(vector<CastExpression> exprs);

    void addViolation(const string& vio);

    void printViolations();

    void checkClassFields(vector<FieldDeclaration> fields);

    string getFileName(string& filePath);

    bool isClassPublic(ClassDeclaration classname);

    void checkLiterals(vector<Literal> literls);   
};