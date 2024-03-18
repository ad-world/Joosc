#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <variant>
#include <optional>
#include "scope.h"
#include "type-decl/linkedtype.h"
#include "type-decl/type_declaration.h"
#include <iostream>
#include <list>

class SymbolTable;
struct QualifiedIdentifier;
struct Identifier;

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

    std::optional<TypeDeclaration> tryFindTypeInPackage(std::string &identifier);
    std::optional<PackageDeclarationObject*> tryFindPackageInPackage(QualifiedIdentifier &qualified_identifier);

    PackageDeclarationObject(const std::string &identifier);
    PackageDeclarationObject();

    // Helpers
    ClassDeclarationObject* getJavaLangObject();
    PackageDeclarationObject* findPackageDeclaration(std::vector<Identifier> &identifiers);
    ClassDeclarationObject* findClassDeclaration(std::vector<Identifier> &identifiers);
    InterfaceDeclarationObject* findInterfaceDeclaration(std::vector<Identifier> &identifiers);

    PackageDeclarationObject* findPackageDeclaration(std::string str);
    ClassDeclarationObject* findClassDeclaration(std::string str);
    InterfaceDeclarationObject* findInterfaceDeclaration(std::string str);
};

// Common fields for class and interface
struct TypeDeclarationObject {
    std::unordered_map<std::string, MethodDeclarationObject*> all_methods; // declared and inherited methods
    std::unordered_map<std::string, std::list<MethodDeclarationObject*>> overloaded_methods; // declared and inherited methods
    std::unique_ptr<SymbolTable> methods; // SymbolTable mapping to MethodDeclarationObjects

    PackageDeclarationObject* package_contained_in; // Back-link to the package that contains this type

    TypeDeclarationObject();

    virtual ~TypeDeclarationObject() = default;
    TypeDeclarationObject(TypeDeclarationObject&&) = default;
    TypeDeclarationObject &operator=(TypeDeclarationObject&&) = default;
};

struct ClassDeclarationObject : public TypeDeclarationObject {
    std::string identifier;
    class ClassDeclaration* ast_reference = nullptr;

    std::unique_ptr<SymbolTable> fields; // SymbolTable mapping to FieldDeclarationObjects

    // Declared and inherited fields, aside from those that are shadowed
    std::unordered_map<std::string, FieldDeclarationObject*> accessible_fields;

    void printAllMethods() {
        for (auto it: all_methods) {
            std::cout << it.first << std::endl;
        }
    }

    // Fields resolved at type linking stage
    ClassDeclarationObject* extended = nullptr;
    std::vector<InterfaceDeclarationObject*> implemented;

    // Determine if this is a subtype of another class/interface
    bool isSubType(TypeDeclaration);

    ClassDeclarationObject(const std::string &identifier);
};

struct InterfaceDeclarationObject : public TypeDeclarationObject {
    std::string identifier;
    class InterfaceDeclaration* ast_reference = nullptr;

    // Fields resolved at type linking stage
    std::vector<InterfaceDeclarationObject*> extended;

    // Determine if this is a subtype of another class/interface
    bool isSubType(TypeDeclaration);

    InterfaceDeclarationObject(const std::string &identifier);
};

struct FieldDeclarationObject {
    std::string identifier;
    class FieldDeclaration* ast_reference = nullptr;

    ClassDeclarationObject* containing_class;

    // Fields resolved at type linking stage
    LinkedType type;

    FieldDeclarationObject(const std::string &identifier);
};

struct MethodDeclarationObject {
    std::string identifier;
    class MethodDeclaration* ast_reference = nullptr;

    std::unique_ptr<SymbolTable> parameters; // SymbolTable mapping to FormalParameterDeclarationObject
    std::vector<FormalParameterDeclarationObject*> parameter_list;
    LocalVariableScopeManager scope_manager; // Manager of SymbolTables mapping to LocalVariableDeclarationObject

    std::vector<FormalParameterDeclarationObject*> getParameters();

    bool is_constructor = false;

    // Fields resolved at type linking stage
    LinkedType return_type;

    TypeDeclarationObject* containing_type; // Back-link to the type that contains this method

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
