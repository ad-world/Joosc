#pragma once
#include <variant>
#include "type-decl/type_declaration.h"
#include "variant-ast/primitivetype.h"

using NonArrayLinkedType 
    = std::variant<PrimitiveType, struct ClassDeclarationObject*, struct InterfaceDeclarationObject*, std::nullptr_t>;

struct LinkedType {
    bool is_array;
    NonArrayLinkedType linked_type;

    LinkedType() : is_array{false}, linked_type{nullptr} {}
    LinkedType(TypeDeclaration type_declaration) {
        std::visit([&](auto type_dec){ this->linked_type = type_dec; }, type_declaration);
    }
    // Return whether the type stored is a primitive type rather than a class/interface link
    bool isPrimitive() {
        return bool(std::get_if<PrimitiveType>(&linked_type));
    }
};
