#include "primitivetype.h"
#include "exceptions/exceptions.h"

std::string getPrimitiveName(PrimitiveType pt) {
    switch(pt) {
        case PrimitiveType::BYTE:
            return "byte";
        case PrimitiveType::SHORT:
            return "short";
        case PrimitiveType::INT:
            return "int";
        case PrimitiveType::CHAR:
            return "char";
        case PrimitiveType::BOOLEAN:
            return "boolean";
        case PrimitiveType::VOID:
            return "void";
        case PrimitiveType::NULL_T:
            return "null";
        default:
            THROW_CompilerError("Unrecognized primitive type");
    }
}
