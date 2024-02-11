#include "variant-ast/astnode.h"
#include <memory>
#include <vector>
#include <gtest/gtest.h>


TEST(AstNode, ConstructorCompiles) {
    auto package_declaration = std::unique_ptr<QualifiedIdentifier>();
    auto single_imports = std::vector<QualifiedIdentifier>();
    auto asterisk_imports = std::vector<QualifiedIdentifier>();
    auto class_declarations = std::vector<ClassDeclaration>();
    auto interface_declarations = std::vector<InterfaceDeclaration>();

    auto root = CompilationUnit(
        package_declaration, 
        single_imports, 
        asterisk_imports, 
        class_declarations, 
        interface_declarations
    );
}
