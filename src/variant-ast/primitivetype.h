#pragma once
#include <string>

enum class PrimitiveType {
    BYTE, SHORT, INT, CHAR, BOOLEAN, VOID
};

std::string getPrimitiveName(PrimitiveType pt);
