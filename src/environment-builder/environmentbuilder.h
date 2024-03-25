#pragma once

#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "exceptions/semanticerror.h"
#include "type-decl/type_declaration.h"


// Visit each declaration in an AST, and build an environment for it.
// Ignore non declaration nodes and pass through them.
class EnvironmentBuilder : public DefaultSkipVisitor<void> {

    // Used to track current declaration new declarations are inside, so e.g. methods can be added to symbol
    // table for current class
    class PackageDeclarationObject *current_package;
    TypeDeclaration current_type;
    class MethodDeclarationObject *current_method;

    // Link AstNode with its declaration object, giving both pointers to each other
    template <typename AstNodeType, typename SymbolTableEntryType> 
    void linkDeclaration(AstNodeType &node, SymbolTableEntryType &env) {
        node.environment = &env;
        env.ast_reference = &node;
    }

  public:
    // Should never be called on their own in this class, but must be public for std::visit
    using DefaultSkipVisitor<void>::operator();

    void operator()(CompilationUnit &node) override;

    void operator()(ClassDeclaration &node) override;

    void operator()(InterfaceDeclaration &node) override;

    void operator()(FieldDeclaration &node) override;

    void operator()(MethodDeclaration &node) override;

    void operator()(Block &node) override;

    void operator()(ForStatement &node) override;

    void operator()(FormalParameter &node) override;

    void operator()(LocalVariableDeclaration &node) override;

    EnvironmentBuilder(PackageDeclarationObject &default_package);

    void visit(AstNodeVariant &node) override;
};
