#include <memory>
#include <vector>
#include <gtest/gtest.h>

#include "variant-ast/astnode.h"

TEST(AstNodeVariant, LValueConstructorCompiles) {
    auto identifiers = std::vector<Identifier>();
    auto package_declaration = std::make_unique<QualifiedIdentifier>(
        identifiers
    );
    auto single_imports = std::vector<QualifiedIdentifier>();
    auto asterisk_imports = std::vector<QualifiedIdentifier>();
    auto class_declarations = std::vector<ClassDeclaration>();
    auto interface_declarations = std::vector<InterfaceDeclaration>();

    AstNodeVariant root = CompilationUnit(
        package_declaration, 
        single_imports, 
        asterisk_imports, 
        class_declarations, 
        interface_declarations
    );
}

TEST(AstNodeVariant, RValueConstructorCompiles) {
    AstNodeVariant root = CompilationUnit(
        std::make_unique<QualifiedIdentifier>(
            std::vector<Identifier>()
        ),
        std::vector<QualifiedIdentifier>(), 
        std::vector<QualifiedIdentifier>(), 
        std::vector<ClassDeclaration>(), 
        std::vector<InterfaceDeclaration>()
    );
}
