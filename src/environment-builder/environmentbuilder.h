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
        for (auto &identifier : package_name.identifiers) {
            current_environment = current_environment->localLookupOrCreate(identifier.name, Environment::PACKAGE);
        }
    }

    void operator()(CompilationUnit &node) override {
        lookupOrCreatePackage(*node.package_declaration);
    }

    void operator()(ClassDeclaration &node) override {
        
    }

    void operator()(InterfaceDeclaration &node) override;

    void operator()(MethodDeclaration &node) override;

    void operator()(FieldDeclaration &node) override;

    void operator()(LocalVariableDeclaration &node) override;

    void operator()(Block &node) override;

  public:
    // env: Evironment to populate with pass of AST
    EnvironmentBuilder(Environment *env) 
        : program_environment{env}, current_environment{env} {};

    void visit(AstNodeVariant &node) {
        std::visit(*this, node);
        current_environment = nullptr;
    }
};
