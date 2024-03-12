#include "environment-builder/symboltableentry.h"
#include "variant-ast/astnode.h"

struct Main {
    static PackageDeclarationObject* root_package;
    static std::vector<AstNodeVariant>* linked_asts;
};
