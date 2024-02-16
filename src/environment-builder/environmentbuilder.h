#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "exceptions/semanticerror.h"

class Environment;

// Visit each declaration in an AST, and build an environment for it.
// Ignore non declaration nodes and pass through them.
class EnvironmentBuilder : public DefaultSkipVisitor<void> {
    Environment* program_environment;
    Environment* current_environment;

    // Traverses from the current environment to the fully qualified package
    // environment if it exists, or adds it to the current environment.
    //
    // Throws SemanticError if name conflicts with a class.
    void lookupOrCreatePackage(QualifiedIdentifier &package_name) {
        // for (auto &identifier : package_name.identifiers) {
        //     current_environment = current_environment->localLookupOrCreate(identifier.name, Environment::PACKAGE);
        // }
    }

    void operator()(CompilationUnit &node) override {
        if (node.package_declaration) {
            // Walk down symbol table and set current symbol table to table of package
            lookupOrCreatePackage(*node.package_declaration);
        } else {
            // CompilationUnit is using the default package
        }
        visit_children(node);
    }

    void operator()(ClassDeclaration &node) override {
        // Insert class in current package table

        // Set current table to class's method table
        for (auto &method_dec : node.method_declarations) {
            this->operator()(method_dec);
        }
        // Set current table to class's field table
        for (auto &field_dec : node.field_declarations) {
            this->operator()(field_dec);
        }

        // Set current table back to current package table
    }

    void operator()(InterfaceDeclaration &node) override {
        visit_children(node);
    }

    void operator()(FieldDeclaration &node) override {
        visit_children(node);
    }

    void operator()(MethodDeclaration &node) override {
        visit_children(node);
    }

    void operator()(FormalParameter &node) override {
        visit_children(node);
    }

    void operator()(Block &node) override {
        // Open scope
        visit_children(node);
        // Close scope
    }

    void operator()(LocalVariableDeclaration &node) override {
        visit_children(node);
    }

  public:
    // env: Evironment to populate with pass of AST
    EnvironmentBuilder(Environment *env) 
        : program_environment{env}, current_environment{env} {};

    void visit(AstNodeVariant &node) {
        std::visit(*this, node);
        current_environment = nullptr;
    }
};
