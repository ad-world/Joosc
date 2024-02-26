#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include "scope.h"
#include "type-decl/linkedtype.h"

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

    PackageDeclarationObject(const std::string &identifier);
    PackageDeclarationObject();
};

struct ClassDeclarationObject {
    std::string identifier;
    class ClassDeclaration* ast_reference = nullptr;

    std::unique_ptr<SymbolTable> fields; // SymbolTable mapping to FieldDeclarationObjects
    std::unique_ptr<SymbolTable> methods; // SymbolTable mapping to MethodDeclarationObjects

    // Fields resolved at type linking stage
    ClassDeclarationObject* extended = nullptr;
    std::vector<InterfaceDeclarationObject*> implemented;

    ClassDeclarationObject(const std::string &identifier);
};

struct InterfaceDeclarationObject {
    std::string identifier;
    class InterfaceDeclaration* ast_reference = nullptr;

    std::unique_ptr<SymbolTable> methods; // SymbolTable mapping to MethodDeclarationObjects

    // Fields resolved at type linking stage
    std::vector<InterfaceDeclarationObject*> extended;

    InterfaceDeclarationObject(const std::string &identifier);
};

struct FieldDeclarationObject {
    std::string identifier;
    class FieldDeclaration* ast_reference = nullptr;

    // Fields resolved at type linking stage
    LinkedType type;

    FieldDeclarationObject(const std::string &identifier);
};

struct MethodDeclarationObject {
    std::string identifier;
    class MethodDeclaration* ast_reference = nullptr;

    std::unique_ptr<SymbolTable> parameters; // SymbolTable mapping to FormalParameterDeclarationObject
    LocalVariableScopeManager scope_manager; // Manager of SymbolTables mapping to LocalVariableDeclarationObject

    bool is_constructor;

    // Fields resolved at type linking stage
    LinkedType return_type;

    MethodDeclarationObject(const std::string &identifier);
};

struct FormalParameterDeclarationObject {
    std::string identifier;
    class FormalParameter* ast_reference = nullptr;

    // Fields resolved at type linking stage
    LinkedType type;

    FormalParameterDeclarationObject(const std::string &identifier);
};

struct LocalVariableDeclarationObject {
    std::string identifier;
    class LocalVariableDeclaration* ast_reference = nullptr;

    // Fields resolved at type linking stage
    LinkedType type;

    LocalVariableDeclarationObject(const std::string &identifier);
};
