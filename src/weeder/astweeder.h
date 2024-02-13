#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <variant>
#include <fstream>
#include "variant-ast/astnode.h"
#include "variant-ast/astvisitor/graballvisitor.h"

using namespace std;
class AstWeeder {
public: 
    int weed(AstNodeVariant& root, string fileName);

    AstWeeder();

private:
    vector<string> violations;

    void checkAsciiRange(const string& source);

    void checkClassModifiersAndConstructors(vector<ClassDeclaration> &classes, string filename);

    void checkInterfaces(vector<InterfaceDeclaration> &interfaces, string filename);

    void checkMethodModifiersAndBody(const vector<MethodDeclaration> &methods);

    void addViolation(const string& vio);

    void printViolations();

    void checkClassFields(const vector<FieldDeclaration> &fields);

    void checkLiterals(vector<Literal*> literls, vector<PrefixExpression*> prefixExpr);   
};