#pragma once
#include <variant>

class Expression {};

typedef std::variant<std::monostate, Expression> ASTNode;
