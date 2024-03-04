#pragma once

#include <variant>
#include <memory>
#include "type-decl/linkedtype.h"

#include "primitivetype.h"

typedef std::variant<PrimitiveType, struct QualifiedIdentifier> NonArrayType;

struct Type {
    std::unique_ptr<NonArrayType> non_array_type;
    bool is_array;

    LinkedType link;

    Type(std::unique_ptr<NonArrayType>& non_array_type, bool is_array);
    Type(std::unique_ptr<NonArrayType>&& non_array_type, bool is_array);

    // Operators
    friend bool operator==(const Type & lhs, const Type & rhs);
    friend bool operator!=(const Type & lhs, const Type & rhs);
};
