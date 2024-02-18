#include <variant>
#include "utillities/overload.h"
#include "environment-builder/symboltableentry.h"
#include "exceptions/compilerdevelopmenterror.h"
#include "get_as_type_dec.h"

namespace util{

TypeDeclaration getAsTypeDeclaration(SymbolTableEntry *entry) {
    return std::visit(overload {
        [&] (ClassDeclarationObject &obj) -> TypeDeclaration { return &obj; },
        [&] (InterfaceDeclarationObject &obj) -> TypeDeclaration { return &obj; },
        [&] (auto &obj) -> TypeDeclaration { throw CompilerDevelopmentError("entry was not a type declaration"); }
    }, *entry);
}

};
