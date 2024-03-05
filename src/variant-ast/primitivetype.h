#pragma once
#include <string>

enum class PrimitiveType {
    BYTE, SHORT, INT, CHAR, BOOLEAN, VOID, NULL_T
};

std::string getPrimitiveName(PrimitiveType pt);
