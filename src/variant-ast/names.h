#pragma once

#include <string>
#include <vector>
#include "astnodecommon.h"
#include "astnode.h"
#include <unordered_map>

enum Classification {
    EXPRESSION_NAME,
    TYPE_NAME,
    PACKAGE_NAME,
    METHOD_NAME,
    UNCLASSIFIED
};

std::string inline classificationToString(Classification classification) {
    switch (classification) {
        case EXPRESSION_NAME: return "EXPRESSION_NAME";
        case TYPE_NAME: return "TYPE_NAME";
        case PACKAGE_NAME: return "PACKAGE_NAME";
        case METHOD_NAME: return "METHOD_NAME";
        case UNCLASSIFIED: return "UNCLASSIFIED";
        default: return "ERROR CLASSIFICATION NOT FOUND";
    }
}

struct Identifier: public AstNodeCommon {
    std::string name; // Identifier name

    // If this identifier is classified as an PACKAGE_NAME, points to the package it refers to, and nullptr otherwise.
    //
    // Resolved during Disambiguation.
    PackageDeclarationObject* package = nullptr;

    // If this identifier is classified as an EXPRESSION_NAME and the expression is a field,
    // points to the field it refers to, and nullptr otherwise.
    //
    // Resolved during Type Checking.
    FieldDeclarationObject* field = nullptr;

    // If this identifier is classified as an EXPRESSION_NAME and the expression is a variable,
    // points to the variable it refers to, and nullptr otherwise.
    //
    // Resolved during Type Checking.
    LocalVariableDeclarationObject* variable = nullptr;

    // If this identifier is classified as an EXPRESSION_NAME and the expression is a formal parameter,
    // points to the parameter it refers to, and nullptr otherwise.
    //
    // Resolved during Type Checking.
    FormalParameterDeclarationObject* parameter = nullptr;

    // If this identifier is classified as an EXPRESSION_NAME and the expression is the length of an array,
    // is true. This should be the case if all the above are null and this is an EXPRESSION_NAME.
    //
    // Resolved during Type Checking.
    bool is_array_length = false;

    Classification classification = UNCLASSIFIED;

    Identifier(std::string& name) : name(std::move(name)) {}
    Identifier(std::string&& name) : name(std::move(name)) {}
};

struct QualifiedIdentifier: public ExpressionCommon {
    std::vector<Identifier> identifiers; // Vector of identifiers for this qualified identifier

    QualifiedIdentifier() {}
    QualifiedIdentifier(std::vector<Identifier>& identifiers) : identifiers(std::move(identifiers)) {}
    QualifiedIdentifier(std::vector<Identifier>&& identifiers) : identifiers(std::move(identifiers)) {}

public:
    // bool is_array_length = false;

    Classification getClassification() { return identifiers.back().classification; }

    void setRefersTo(FieldDeclarationObject* dec) { identifiers.back().field = dec; }
    void setRefersTo(LocalVariableDeclarationObject* dec) { identifiers.back().variable = dec; }
    void setRefersTo(FormalParameterDeclarationObject* dec) { identifiers.back().parameter = dec; }
    void setRefersToArrayLength() { identifiers.back().is_array_length = true; }

    FieldDeclarationObject* getIfRefersToField() { return identifiers.back().field; }
    LocalVariableDeclarationObject* getIfRefersToLocalVariable() { return identifiers.back().variable; }
    FormalParameterDeclarationObject* getIfRefersToParameter() { return identifiers.back().parameter; }
    bool refersToArrayLength() { return identifiers.back().is_array_length; }

    bool isSimple() { return identifiers.size() == 1; }

    // Returns the stringified qualified name, as accessed.
    // The actual fully quallified name of the underlying object may be different.
    std::string getQualifiedName();

    // Returns the stringified qualified name, of the underlying object.
    // Throws if this is not an object with an underlying qualified name.
    std::string getFullUnderlyingQualifiedName();

    // Returns a copy of the qualified name, as accessed, without the simple name.
    // Undefined behaviour if the qualified identifier is empty.
    QualifiedIdentifier getQualifiedIdentifierWithoutLast();

    std::string getPackagePrefix();
};
