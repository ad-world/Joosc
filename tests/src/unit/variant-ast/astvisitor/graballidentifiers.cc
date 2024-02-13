#include <memory>
#include <vector>
#include <string>
#include <gtest/gtest.h>

#include "variant-ast/astnode.h"
#include "variant-ast/astvisitor/graballvisitor.h"


TEST(GrabAllVisitor, GetsCorrectAmount) {
    std::string name1 = "hello";
    std::string name2 = "hello";
    auto id1 = Identifier(name1);
    auto id2 = Identifier(name2);
    auto identifiers = std::vector<Identifier>{name1, name2};
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

    auto grabber = GrabAllVisitor<Identifier>();
    auto result = grabber(root);
    
    EXPECT_TRUE(result.size() == 2) << "GrabAllVisitor didn't grab all";
}
