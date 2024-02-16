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
    bool is_default_package;

    std::unique_ptr<SymbolTable> sub_packages;
    std::unique_ptr<SymbolTable> classes;
    std::unique_ptr<SymbolTable> interfaces;

    PackageDeclarationObject(const std::string &identifier) : identifier{identifier} {}
    PackageDeclarationObject(const std::string &identifier, bool is_default_package) : 
        identifier{identifier}, is_default_package{is_default_package} {}
};

struct ClassDeclarationObject {
    std::string identifier;
    class ClassDeclaration* ast_reference;

    std::unique_ptr<SymbolTable> fields; // SymbolTable mapping to FieldDeclarationObjects
    std::unique_ptr<SymbolTable> methods; // SymbolTable mapping to MethodDeclarationObjects

    // Fields resolved at type linking stage
    ClassDeclarationObject* extended;
    std::vector<InterfaceDeclarationObject*> implemented;

    ClassDeclarationObject(const std::string &identifier) : identifier{identifier} {}
};

struct InterfaceDeclarationObject {
    std::string identifier;
    class InterfaceDeclaration* ast_reference;

    std::unique_ptr<SymbolTable> methods; // SymbolTable mapping to MethodDeclarationObjects

    // Fields resolved at type linking stage
    std::vector<InterfaceDeclarationObject*> extended;

    InterfaceDeclarationObject(const std::string &identifier) : identifier{identifier} {}
};

struct FieldDeclarationObject {
    std::string identifier;
    class FieldDeclaration* ast_reference;

    // Fields resolved at type linking stage
    SymbolTableEntry *type;

    FieldDeclarationObject(const std::string &identifier) : identifier{identifier} {}
};

struct MethodDeclarationObject {
    std::string identifier;
    class MethodDeclaration* ast_reference;

    std::unique_ptr<SymbolTable> parameters; // SymbolTable mapping to FormalParameterDeclarationObject
    std::unique_ptr<SymbolTable> local_variables; // SymbolTable mapping to LocalVariableDeclarationObject

    // Fields resolved at type linking stage
    SymbolTableEntry *return_type;

    MethodDeclarationObject(const std::string &identifier) : identifier{identifier} {}
};

struct FormalParameterDeclarationObject {
    std::string identifier;
    class FormalParameter* ast_reference;

    // Fields resolved at type linking stage
    SymbolTableEntry *type;

    FormalParameterDeclarationObject(const std::string &identifier) : identifier{identifier} {}
};

struct LocalVariableDeclarationObject {
    std::string identifier;
    class LocalVariableDeclaration* ast_reference;

    // Fields resolved at type linking stage
    SymbolTableEntry *type;

    LocalVariableDeclarationObject(const std::string &identifier) : identifier{identifier} {}
};
