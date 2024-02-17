#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "exceptions/semanticerror.h"
#include "variant-ast/astnode.h"
#include "environment-builder/symboltableentry.h"
#include "environment-builder/environmentbuilder.h"
#include <vector>
#include <string>
#include <variant>

using namespace std;

class TypeLinker : public DefaultSkipVisitor<void> {
    PackageDeclarationObject &root_env;
    CompilationUnit *ast_root;
    vector<QualifiedIdentifier> single_type_import_declarations;
    vector<AstNodeVariant>& asts;
    vector<QualifiedIdentifier> type_import_on_demand_declarations;
    string package_name;

public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(CompilationUnit &node) override;
    void operator()(Type &node) override;
    void operator()(ClassInstanceCreationExpression &node) override;
    TypeLinker(PackageDeclarationObject &root_env, CompilationUnit &ast_root, vector<AstNodeVariant> &asts);

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
