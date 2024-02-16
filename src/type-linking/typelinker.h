#pragma once

#include "defaultskipvisitor.h"
#include "exceptions/semanticerror.h"
#include "variant-ast/astnode.h"
#include "symboltableentry.h"
#include "environmentbuilder.h"
#include <vector>
#include <string>
#include <variant>

using namespace std;

class TypeLinker : public DefaultSkipVisitor<void> {
    PackageDeclarationObject &root_env;
    CompilationUnit *ast_root;
    vector<QualifiedIdentifier> single_type_import_declarations;
    vector<CompilationUnit> asts;
    vector<QualifiedIdentifier> type_import_on_demand_declarations;
    string package_name;

public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(CompilationUnit &node) override;
    void operator()(Type &node) override;
    TypeLinker(PackageDeclarationObject &root_env, CompilationUnit *ast_root, vector<CompilationUnit> &asts);

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};