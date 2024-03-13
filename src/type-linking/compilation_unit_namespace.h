#pragma once

#include <vector>
#include <string>
#include "environment-builder/symboltableentry.h"
struct QualifiedIdentifier;

// Class that manages lookups for the accessible names in a compilation unit, given its package, declared type, and imports.
// Generally throws if lookups are ambiguous.
class CompilationUnitNamespace {
    PackageDeclarationObject *current_package;
    PackageDeclarationObject *default_package;
    TypeDeclaration declared_type;
    std::vector<TypeDeclaration> single_imports;
    std::vector<PackageDeclarationObject*> star_imports;

    // Look up identifier as a type in compilation unit's namespace.
    // Returns all valid candidates.
    std::vector<TypeDeclaration> lookupSimpleType(std::string &identifier);
  public:
    // Look up qualifed_identifier as a type in compilation unit's namespace.
    // Throws semantic error if there are multiple candidates in the namespace.
    TypeDeclaration lookupQualifiedType(QualifiedIdentifier &qualified_identifier);
    TypeDeclaration getDeclaredType() { return declared_type; }
    PackageDeclarationObject* getCurrentPackage() { return current_package; }

    CompilationUnitNamespace() = default;
    CompilationUnitNamespace(
        PackageDeclarationObject *current_package,
        PackageDeclarationObject *default_package,
        TypeDeclaration declared_type,
        std::vector<TypeDeclaration> single_imports,
        std::vector<PackageDeclarationObject*> star_imports
    );
};
