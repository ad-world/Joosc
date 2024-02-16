#include <vector>
#include <gtest/gtest.h>

#include "variant-ast/astnode.h"
#include "type-linking/typelinker.h"

TEST(TypeLinker, BasicTypeLink) {
   // Create the root environment
    std::unique_ptr<Environment> root = std::make_unique<Environment>(nullptr);
    std::unique_ptr<Environment> packageEnv = std::make_unique<PackageEnvironment>(root);
    root->addChild(packageEnv.get());

    // Create 4 classes and add them to the package environment
    for (int i = 0; i < 4; i++) {
        std::string className = "Class" + std::to_string(i);
        Environment* extends = nullptr;
        std::vector<Environment*> implements;

        ClassEnvironment* classEnv = new ClassEnvironment(root, className, extends, implements);
        packageEnv->addChild(classEnv);
    }

    // Create an AST for each class
    std::vector<CompilationUnit> asts;
    for (int i = 0; i < 4; i++) {
        auto identifiers = std::vector<Identifier>();
        auto package_declaration = std::make_unique<QualifiedIdentifier>(
            identifiers
        );
        auto single_imports = std::vector<QualifiedIdentifier>();
        auto asterisk_imports = std::vector<QualifiedIdentifier>();
        auto class_declarations = std::vector<ClassDeclaration>();
        auto interface_declarations = std::vector<InterfaceDeclaration>();

        // Create a class declaration
        std::string className = "Class" + std::to_string(i);

        std::vector<Modifier> modifiers;
        std::unique_ptr<Identifier> identifier = std::make_unique<Identifier>(className);
        std::vector<QualifiedIdentifier> implements;
        std::vector<FieldDeclaration> fields;
        std::vector<MethodDeclaration> methods;

        auto classDeclaration = ClassDeclaration(
            std::move(modifiers),
            std::move(identifier),
            nullptr,
            std::move(implements),
            std::move(fields),
            std::move(methods)
        );

        if (i == 3) {
            // Create a field declaration using Class1
            std::vector<Modifier> modifiers;
            std::vector<Identifier> identifiers = {Identifier("Class1")};
            std::unique_ptr<Type> type = std::make_unique<Type>(QualifiedIdentifier(identifiers), false);
            std::unique_ptr<Identifier> identifier = std::make_unique<Identifier>("field");
            // Create a VariableDeclarator, (unique_ptr)
            std::unique_ptr<VariableDeclarator> fieldDeclarator = std::make_unique<VariableDeclarator>(
                std::move(identifier),
                nullptr
            );

            auto fieldDelc = FieldDeclaration(
                std::move(modifiers),
                std::move(type),
                std::move(fieldDeclarator)
            );

            fields.push_back(fieldDelc);
        }

        // At this point, class 3 has a field declaration that uses Class1
        class_declarations.push_back(classDeclaration);

        asts.push_back(CompilationUnit(
            package_declaration, 
            single_imports, 
            asterisk_imports, 
            class_declarations, 
            interface_declarations
        ));
    }

    // 

}