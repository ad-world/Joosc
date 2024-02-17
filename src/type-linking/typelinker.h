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
    PackageDeclarationObject &root_env;
    CompilationUnit *ast_root;
    vector<QualifiedIdentifier> single_type_import_declarations;
    vector<AstNodeVariant>& asts;
    vector<QualifiedIdentifier> type_import_on_demand_declarations;
    string package_name;

    PackageDeclarationObject *current_package;
    PackageDeclarationObject *default_package;
    std::vector<TypeDeclaration> single_imports;
    std::vector<PackageDeclarationObject*> star_imports;
    
    std::unordered_map<std::string, TypeDeclaration> simple_types_available;

    // Resolve qualified identifier to package, or throw semantic error if it resolves to a type
    PackageDeclarationObject* resolveToPackage(QualifiedIdentifier &qualified_identifier);

    // Resolve qualified identifer to type, or throw semantic error if it resolves to a package
    TypeDeclaration resolveToType(QualifiedIdentifier &qualified_identifier);

    // Resolve simple identifer to type
    TypeDeclaration simpleResolveToType(std::string &identifier);

    // Resolve qualified identifier to fully qualified type
    TypeDeclaration resolveToTypeInPackage(QualifiedIdentifier &qualified_identifier);

    void checkTypeOverlap();

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

    TypeLinker(PackageDeclarationObject &root_env, CompilationUnit &ast_root, vector<AstNodeVariant> &asts);

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
