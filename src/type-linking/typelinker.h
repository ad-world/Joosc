#pragma once

#include "defaultskipvisitor.h"
#include "exceptions/semanticerror.h"
#include "variant-ast/astnode.h"
#include "environment_class.h"
#include <vector>
#include <string>
#include <variant>

using namespace std;

class TypeLinker : public DefaultSkipVisitor<void> {
    Environment *environment;
    CompilationUnit *ast_root;
    vector<QualifiedIdentifier> single_type_import_declarations;
    vector<CompilationUnit> asts;
    vector<QualifiedIdentifier> type_import_on_demand_declarations;
    string package_name;

public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(CompilationUnit &node) override;
    void operator()(Type &node) override;
    TypeLinker(Environment *env, CompilationUnit *ast_root, vector<CompilationUnit> &asts);

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};