#pragma once

#include <variant>
#include "environment-builder/symboltableentry.h"
#include "type-decl/type_declaration.h"

namespace util{

// Cast symbol table entry pointer to type declaration.
// The member of *entry must be a ClassDeclarationObject or InterfaceDeclarationObject,
// otherwise an exception will occur.
TypeDeclaration getAsTypeDeclaration(SymbolTableEntry *entry);

};
