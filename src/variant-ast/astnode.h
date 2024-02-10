#pragma once

#include <variant>
#include "statements.h"
#include "types.h"

typedef std::variant<
    struct CompilationUnit
> AstNodeVariant;