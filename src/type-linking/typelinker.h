#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "exceptions/semanticerror.h"
#include "variant-ast/astnode.h"
#include "environment-builder/symboltableentry.h"
#include "environment-builder/environmentbuilder.h"
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>

using namespace std;

class TypeLinker : public DefaultSkipVisitor<void> {
    PackageDeclarationObject *current_package;
    PackageDeclarationObject *default_package;
    TypeDeclaration current_type;
    std::vector<TypeDeclaration> single_imports;
    std::vector<PackageDeclarationObject*> star_imports;

    // Resolve qualified_identifier to package from source_package.
    // Throws semantic error if any prefix, including the full name, resolves to a type
    // not in source_package.
    PackageDeclarationObject* resolveToPackage(
        QualifiedIdentifier &qualified_identifier, 
        PackageDeclarationObject* source_package
    );

    // Resolve qualified_identifier to fully qualified type from default_package.
    // Throws semantic error if any strict prefix resolves to a type.
    TypeDeclaration resolveToType(QualifiedIdentifier &qualified_identifier);

    // Look up qualifed_identifier as a type in compilation unit's namespace.
    // Throws semantic error if there are multiple candidates in the namespace.
    TypeDeclaration lookupQualifiedType(QualifiedIdentifier &qualified_identifier);

    // Look up identifier as a type in compilation unit's namespace.
    // Throws semantic error if there are multiple candidates in the namespace.
    TypeDeclaration lookupSimpleType(std::string &identifier);

public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(CompilationUnit &node) override;
    void operator()(Type &node) override;
    void operator()(ClassInstanceCreationExpression &node) override;
    void operator()(ClassDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;
    void operator()(FieldDeclaration &node) override;
    void operator()(MethodDeclaration &node) override;
    void operator()(FormalParameter &node) override;
    void operator()(LocalVariableDeclaration &node) override;

    TypeLinker(PackageDeclarationObject &default_package);

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
