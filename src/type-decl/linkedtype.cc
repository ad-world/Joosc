#include "linkedtype.h"
#include "environment-builder/symboltableentry.h"

LinkedType::LinkedType(TypeDeclarationObject* type_declaration, bool is_array, bool not_expression) 
    : is_array{is_array}, not_expression{not_expression} {
    if (ClassDeclarationObject* cls = dynamic_cast<ClassDeclarationObject*>(type_declaration)) {
        this->linked_type = cls;
    } else {
        this->linked_type = dynamic_cast<InterfaceDeclarationObject*>(type_declaration);
    }
}

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

    // Non equal primitive types are not subtypes
    if (this->isPrimitive() || other.isPrimitive()) {
        return false;
    }

    // Both types now must be reference types

    // Array is subtype of other array type, if and only if contained is
    if (this->is_array && other.is_array) {
        // Call recursively without array being set to true
        return LinkedType(this->linked_type).isSubType(LinkedType(other.linked_type), default_package);
    }

    // Any array is only subtype of 3 non-array types: Object, Java.io.Serializable, or Cloneable
    if (this->is_array) {
        if (other.getIfNonArrayIsClass() == default_package->findClassDeclaration("java.lang.Object")) {
            return true;
        } else if (other.getIfNonArrayIsInterface() == default_package->findInterfaceDeclaration("java.lang.Cloneable")) {
            return true;
        } else if (other.getIfNonArrayIsInterface() == default_package->findInterfaceDeclaration("java.io.Serializable")) {
            return true;
        }

        return false;
    }

    // Both types are declared types (class or interface)
    TypeDeclaration this_type = this->getIfNonArrayIsClass();
    if (this->getIfNonArrayIsInterface()) { 
        this_type = this->getIfNonArrayIsInterface(); 
    }

    TypeDeclaration other_type = other.getIfNonArrayIsClass();
    if (other.getIfNonArrayIsInterface()) { 
        other_type = other.getIfNonArrayIsInterface(); 
    }
    
    // Determine if class/interface is subclass/subinterface by calling into SymbolTableEntry's logic
    return std::visit([&](auto type_dec){ return type_dec->isSubType(other_type); }, this_type);
}

std::list<struct MethodDeclarationObject*> LinkedType::getAllMethods(std::string& method_name) {
    // TODO : handle arrays specially
    if (getIfNonArrayIsPrimitive()) { return {}; }

    if (auto cls = getIfNonArrayIsClass()) {
        return cls->overloaded_methods[method_name];
    } else {
        auto ifc = getIfNonArrayIsInterface();
        return ifc->overloaded_methods[method_name];
    }
}

std::string LinkedType::toSimpleString() {
    std::string result = "";
    if (auto prim = getIfNonArrayIsPrimitive()) {
        result += getPrimitiveName(*prim);
    } else if (auto cls = getIfNonArrayIsClass()) {
        result += cls->identifier;
    } else if (auto ifc = getIfNonArrayIsInterface()) {
        result += ifc->identifier;
    } else {
        result += "!UNINITIALIZED_LINK_TYPE_LIKELY_ERROR";
    }
    if (is_array) {
        result += "[]";
    }
    return result;
}