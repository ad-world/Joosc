#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>

class SymbolTable;

using SymbolTableEntry = std::variant<
    struct PackageDeclarationObject,
    struct ClassDeclarationObject,
    struct InterfaceDeclarationObject,
    struct FieldDeclarationObject,
    struct MethodDeclarationObject,
    struct FormalParameterDeclarationObject,
    struct LocalVariableDeclarationObject
>;

struct PackageDeclarationObject {
    std::string identifier;

    std::unique_ptr<SymbolTable> sub_packages;
    std::unique_ptr<SymbolTable> classes;
    std::unique_ptr<SymbolTable> interfaces;
};

struct ClassDeclarationObject {
    std::string identifier;
    class ClassDeclaration* ast_reference;

    std::unique_ptr<SymbolTable> fields; // SymbolTable mapping to FieldDeclarationObjects
    std::unique_ptr<SymbolTable> methods; // SymbolTable mapping to MethodDeclarationObjects

    // Fields resolved at type linking stage
    ClassDeclarationObject* extended;
    std::vector<InterfaceDeclarationObject*> implemented;
};

struct InterfaceDeclarationObject {
    std::string identifier;
    class InterfaceDeclaration* ast_reference;

    std::unique_ptr<SymbolTable> methods; // SymbolTable mapping to MethodDeclarationObjects

    // Fields resolved at type linking stage
    std::vector<InterfaceDeclarationObject*> extended;
};

struct FieldDeclarationObject {
    std::string identifier;
    class FieldDeclaration* ast_reference;

    // Fields resolved at type linking stage
    SymbolTableEntry *type;
};

struct MethodDeclarationObject {
    std::string identifier;
    class MethodDeclaration* ast_reference;

    std::unique_ptr<SymbolTable> parameters; // SymbolTable mapping to FormalParameterDeclarationObject

    // Fields resolved at type linking stage
    SymbolTableEntry *return_type;
};

struct FormalParameterDeclarationObject {
    std::string identifier;
    class FormalParameter* ast_reference;

    // Fields resolved at type linking stage
    SymbolTableEntry *type;
};

struct LocalVariableDeclarationObject {
    std::string identifier;
    class LocalVariableDeclaration* ast_reference;

    // Fields resolved at type linking stage
    SymbolTableEntry *type;
};
