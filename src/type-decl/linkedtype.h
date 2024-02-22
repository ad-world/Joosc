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
};
