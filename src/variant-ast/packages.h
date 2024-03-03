#pragma once

#include <vector>
#include <memory>
#include "astnodecommon.h"
#include "type-linking/compilation_unit_namespace.h"

struct QualifiedIdentifier;
struct ClassDeclaration;
struct InterfaceDeclaration;

struct CompilationUnit: public AstNodeCommon {
    std::unique_ptr<QualifiedIdentifier> package_declaration; // The singular package declaration
    std::vector<QualifiedIdentifier> single_type_import_declaration; // All single type imports
    std::vector<QualifiedIdentifier> type_import_on_demand_declaration; // All asterisk imports
    std::vector<ClassDeclaration> class_declarations; // All class declarations
    std::vector<InterfaceDeclaration> interface_declarations; // All import declarations

    CompilationUnitNamespace cu_namespace;

    CompilationUnit(
        std::unique_ptr<QualifiedIdentifier>& package_declaration, 
        std::vector<QualifiedIdentifier>& single_imports, 
        std::vector<QualifiedIdentifier>& asterisk_imports, 
        std::vector<ClassDeclaration>& class_decs, 
        std::vector<InterfaceDeclaration>& interface_decs
    );
    CompilationUnit(
        std::unique_ptr<QualifiedIdentifier>&& package_declaration, 
        std::vector<QualifiedIdentifier>&& single_imports, 
        std::vector<QualifiedIdentifier>&& asterisk_imports, 
        std::vector<ClassDeclaration>&& class_decs, 
        std::vector<InterfaceDeclaration>&& interface_decs
    );
};
