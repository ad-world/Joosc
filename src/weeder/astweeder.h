#pragma once
#include <iostream>
#include <vector>
#include "astnode.h"


using namespace std;
class AstWeeder {
public: 
    int weed(AstNodeVariant* root, string fileName);

    AstWeeder();

private:
    vector<string> violations;

    void checkAsciiRange(const string& source);

    void checkClassModifiersAndConstructors(vector<ClassDeclaration*>, string filename);

    void checkInterfaces(vector<InterfaceDeclaration*>, string filename);

    void checkMethodModifiersAndBody(vector<MethodDeclaration*> methods);

    void checkCastExpressionsValid(CastExpression* expr);

    void addViolation(string& vio);

    void printViolations();

    void checkClassFields(vector<FieldDeclaration*> fields);

    string getFileName(string& filePath);

    bool isClassPublic(ClassDeclaration* class);

    void checkLiterals(vector<Literal*> literls);   
}