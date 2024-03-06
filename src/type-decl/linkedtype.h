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
    LinkedType(NonArrayLinkedType non_array_type) : is_array{false}, linked_type{non_array_type} {}
    LinkedType(TypeDeclaration type_declaration) {
        std::visit([&](auto type_dec){ this->linked_type = type_dec; }, type_declaration);
    }

    // Return whether the type stored is a primitive type rather than a class/interface link or array
    bool isPrimitive() {
        return bool(std::get_if<PrimitiveType>(&linked_type)) && !is_array;
    }
    // Return whether the type stored is an array or a class/interface
    bool isReferenceType() { return !isPrimitive(); }

    // Returns a pointer to the ClassDeclarationObject that is contained in the NonArrayType,
    // if it is, and nullptr otherwise
    ClassDeclarationObject* getIfNonArrayIsClass() {
        if (std::get_if<ClassDeclarationObject*>(&linked_type)) {
            return std::get<ClassDeclarationObject*>(linked_type);
        }
        return nullptr;
    }

    // Returns a pointer to the InterfaceDeclarationObject that is contained in the NonArrayType,
    // if it is, and nullptr otherwise
    InterfaceDeclarationObject* getIfNonArrayIsInterface() {
        if (std::get_if<InterfaceDeclarationObject*>(&linked_type)) {
            return std::get<InterfaceDeclarationObject*>(linked_type);
        }
        return nullptr;
    }

    // Returns a pointer to the PrimitiveType that is contained in the NonArrayType,
    // if it is, and nullptr otherwise
    PrimitiveType* getIfNonArrayIsPrimitive() {
        if (std::get_if<PrimitiveType>(&linked_type)) {
            return &std::get<PrimitiveType>(linked_type);
        }
        return nullptr;
    }
};
