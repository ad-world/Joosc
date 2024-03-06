#include "linkedtype.h"
#include "environment-builder/symboltableentry.h"

ClassDeclarationObject* LinkedType::getIfNonArrayIsClass() {
    if (std::get_if<ClassDeclarationObject*>(&linked_type)) {
        return std::get<ClassDeclarationObject*>(linked_type);
    }
    return nullptr;
}

InterfaceDeclarationObject* LinkedType::getIfNonArrayIsInterface() {
    if (std::get_if<InterfaceDeclarationObject*>(&linked_type)) {
        return std::get<InterfaceDeclarationObject*>(linked_type);
    }
    return nullptr;
}

PrimitiveType* LinkedType::getIfNonArrayIsPrimitive() {
    if (std::get_if<PrimitiveType>(&linked_type)) {
        return &std::get<PrimitiveType>(linked_type);
    }
    return nullptr;
}

bool LinkedType::isSubType(LinkedType other, PackageDeclarationObject* default_package) {
    // Type is trivally subtype of itself
    if (*this == other) { return true; }

    if (this->isPrimitive() || other.isPrimitive()) {
        // Non equal primitive types are not subtypes
        return false;
    }

    // Both types now must be reference types

    if (this->is_array && other.is_array) {
        // Array is subtype of other array type, if and only if contained is
        // Call recursively without array being set to true
        return LinkedType(this->linked_type).isSubType(LinkedType(other.linked_type), default_package);
    }

    // All arrays are subtype of Object
    // TODO : I think it needs to be subtype of java.io.Serializable or something as well
    if (this->is_array) {
        if (other.getIfNonArrayIsClass() == default_package->getJavaLangObject()) {
            return true;
        } else {
            // If one type is an array and one isn't, not a subtype
            return false;
        }
    }

    // Both types are declared types (class or interface)
    TypeDeclaration this_type = this->getIfNonArrayIsClass();
    TypeDeclaration other_type = other.getIfNonArrayIsClass();
    if (this->getIfNonArrayIsInterface()) { this_type = this->getIfNonArrayIsInterface(); }
    if (other.getIfNonArrayIsInterface()) { other_type = other.getIfNonArrayIsInterface(); }
    
    // Determine if class/interface is subclass/subinterface by calling into SymbolTableEntry's logic
    return std::visit([&](auto type_dec){ return type_dec->isSubType(other_type); }, this_type);
}