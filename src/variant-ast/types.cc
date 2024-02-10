#include "types.h"

struct QualifiedIdentifier{};

Type::Type(std::unique_ptr<NonArrayType>& non_array_type, bool is_array) :
    non_array_type{std::move(non_array_type)}, is_array{is_array}
{}
