#pragma once

#include <variant>
#include <memory>
#include "type-decl/type_declaration.h"


enum PrimitiveType {
    BYTE, SHORT, INT, CHAR, BOOLEAN, VOID
};

typedef std::variant<PrimitiveType, struct QualifiedIdentifier> NonArrayType;

struct Type {
    TypeDeclaration node;
    std::unique_ptr<NonArrayType> non_array_type;
    bool is_array;

    Type(std::unique_ptr<NonArrayType>& non_array_type, bool is_array);
    Type(std::unique_ptr<NonArrayType>&& non_array_type, bool is_array);
};
