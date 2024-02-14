#include "packages.h"
#include "classes.h"
#include "expressions.h"
#include "statements.h"
#include "types.h"
#include "names.h"

CompilationUnit::CompilationUnit(
    std::unique_ptr<QualifiedIdentifier>& package_declaration, 
    std::vector<QualifiedIdentifier>& single_imports, 
    std::vector<QualifiedIdentifier>& asterisk_imports, 
    std::vector<ClassDeclaration>& class_decs, 
    std::vector<InterfaceDeclaration>& interface_decs
) : 
    package_declaration{std::move(package_declaration)},
    single_type_import_declaration{std::move(single_imports)},
    type_import_on_demand_declaration{std::move(asterisk_imports)},
    class_declarations{std::move(class_decs)},
    interface_declarations{std::move(interface_decs)}
{}

// --------

CompilationUnit::CompilationUnit(
    std::unique_ptr<QualifiedIdentifier>&& package_declaration, 
    std::vector<QualifiedIdentifier>&& single_imports, 
    std::vector<QualifiedIdentifier>&& asterisk_imports, 
    std::vector<ClassDeclaration>&& class_decs, 
    std::vector<InterfaceDeclaration>&& interface_decs
) : 
    package_declaration{std::move(package_declaration)},
    single_type_import_declaration{std::move(single_imports)},
    type_import_on_demand_declaration{std::move(asterisk_imports)},
    class_declarations{std::move(class_decs)},
    interface_declarations{std::move(interface_decs)}
{}
