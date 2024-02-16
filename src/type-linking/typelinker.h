#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "exceptions/semanticerror.h"
#include "environment_class.h"
#include <vector>
#include <string>

using namespace std;

class TypeLinker : public DefaultSkipVisitor<void> {
    Environment *environment;
    CompilationUnit ast_root;
    vector<QualifiedIdentifier> single_type_import_declarations;
    vector<CompilationUnit> asts;
    vector<QualifiedIdentifier> type_import_on_demand_declarations;
    string package_name;

    void operator()(CompilationUnit &node) override;
    void operator()(QualifiedIdentifier &node) override;
public:
    TypeLinker(Environment *env, CompilationUnit *ast_root, vector<CompilationUnit> &asts);
};