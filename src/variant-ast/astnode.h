#pragma once

#include <variant>
#include "packages.h"
#include "classes.h"
#include "expressions.h"
#include "statements.h"
#include "types.h"
#include "names.h"

// Include this header to include all AST classes

typedef std::variant<
    CompilationUnit
> AstNodeVariant;