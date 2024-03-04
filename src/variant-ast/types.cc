#include "types.h"
#include "names.h"

Type::Type(std::unique_ptr<NonArrayType>& non_array_type, bool is_array) :
    non_array_type{std::move(non_array_type)}, is_array{is_array}, link{}
{}

// -------

Type::Type(std::unique_ptr<NonArrayType>&& non_array_type, bool is_array) :
    non_array_type{std::move(non_array_type)}, is_array{is_array}, link{}
{}


bool operator==(const Type & lhs, const Type & rhs) {
    if ( lhs.is_array != rhs.is_array ) {
        return false;
    }

    if ( std::holds_alternative<QualifiedIdentifier>(*lhs.non_array_type) && std::holds_alternative<QualifiedIdentifier>(*rhs.non_array_type) ) {
        return (
        (std::get<QualifiedIdentifier>(*lhs.non_array_type)).getQualifiedName()
        == (std::get<QualifiedIdentifier>(*rhs.non_array_type)).getQualifiedName()
    );
    } else if ( std::holds_alternative<PrimitiveType>(*lhs.non_array_type) && std::holds_alternative<PrimitiveType>(*rhs.non_array_type) ) {
        return (
        std::get<PrimitiveType>(*lhs.non_array_type)
        == std::get<PrimitiveType>(*rhs.non_array_type)
    );
    } else {
        return false;
    }
}

bool operator!=(const Type & lhs, const Type & rhs) {
    return !(lhs == rhs);
}
