#pragma once

#include <variant>
#include <memory>
#include "environmentbuilder.h"
#include "symboltableentry.h"

enum PrimitiveType {
    BYTE, SHORT, INT, CHAR, BOOLEAN, VOID
};

typedef std::variant<PrimitiveType, struct QualifiedIdentifier> NonArrayType;

struct Type {
    TypeDeclaration node; // Todo: change this to ClassDecl that Amrit is working on
    std::unique_ptr<NonArrayType> non_array_type;
    bool is_array;

    Type(std::unique_ptr<NonArrayType>& non_array_type, bool is_array);
    Type(std::unique_ptr<NonArrayType>&& non_array_type, bool is_array);
};
