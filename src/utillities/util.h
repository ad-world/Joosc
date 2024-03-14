#pragma once

#include "get_as_type_dec.h"
#include "overload.h"
#include "remove_duplicates.h"
#include "environment-builder/symboltableentry.h"
#include "variant-ast/astnode.h"

struct Util {
    static PackageDeclarationObject* root_package;
    static std::vector<AstNodeVariant>* linked_asts;
    static std::string locationToString(yy::location &loc);
};
