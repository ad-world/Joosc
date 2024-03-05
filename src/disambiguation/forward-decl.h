#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "variant-ast/expressions.h"
#include "type-decl/type_declaration.h"
#include "type-decl/linkedtype.h"
#include "environment-builder/symboltableentry.h"
#include "environment-builder/symboltable.h"
#include "exceptions/exceptions.h"
#include <variant>
#include <string>

class ForwardDeclarationVisitor : public DefaultSkipVisitor<void> {
    PackageDeclarationObject *default_package = nullptr;
    PackageDeclarationObject *current_package = nullptr;
    MethodDeclarationObject *current_method = nullptr;
    ClassDeclarationObject *current_class = nullptr;
    InterfaceDeclarationObject *current_interface = nullptr;
    CompilationUnit *compilation_unit = nullptr;

    std::string current_identifier;

public:
    ForwardDeclarationVisitor(PackageDeclarationObject &default_package): default_package(&default_package), current_package(&default_package) {}
    
    using DefaultSkipVisitor<void>::operator();
    void operator()(CompilationUnit &node) override;
    void operator()(MethodDeclaration &node) override;
    void operator()(ClassDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;
    void operator()(FieldDeclaration &node) override;
    void operator()(Assignment &node) override;
    void operator()(QualifiedIdentifier &node) override;

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};