#pragma once

#include <memory>
#include <vector>
#include <list>
#include "astnodecommon.h"
#include "expressions.h"

struct Identifier;
struct QualifiedIdentifier;
struct Type;
struct Block;
struct CfgStatement;

enum class Modifier {
    PUBLIC,
    PROTECTED,
    ABSTRACT,
    STATIC,
    NATIVE,
    FINAL
};

struct VariableDeclarator : public AstNodeCommon {
    std::unique_ptr<Identifier> variable_name;
    std::unique_ptr<Expression> expression;

    VariableDeclarator(
        std::unique_ptr<Identifier>& variable_name, 
        std::unique_ptr<Expression>& expression
    );
    VariableDeclarator(
        std::unique_ptr<Identifier>&& variable_name, 
        std::unique_ptr<Expression>&& expression
    );
};

struct FieldDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers;
    std::unique_ptr<Type> type;
    std::unique_ptr<VariableDeclarator> variable_declarator;

    struct FieldDeclarationObject *environment = nullptr;

    bool hasModifier(Modifier mod_to_check);

    FieldDeclaration(
        std::vector<Modifier>& modifiers,
        std::unique_ptr<Type>& type,
        std::unique_ptr<VariableDeclarator>& variable_declarator
    );
    FieldDeclaration(
        std::vector<Modifier>&& modifiers,
        std::unique_ptr<Type>&& type,
        std::unique_ptr<VariableDeclarator>&& variable_declarator
    );
};

struct FormalParameter : public AstNodeCommon {
    std::unique_ptr<Type> type;
    std::unique_ptr<Identifier> parameter_name;

    struct FormalParameterDeclarationObject *environment = nullptr;

    FormalParameter(
        std::unique_ptr<Type>& type, 
        std::unique_ptr<Identifier>& parameter_name
    );
    FormalParameter(
        std::unique_ptr<Type>&& type, 
        std::unique_ptr<Identifier>&& parameter_name
    );

    std::string toString() const;
};

struct MethodDeclaration : public AstNodeCommon {
    std::vector<Modifier> modifiers;
    std::unique_ptr<Type> type;
    std::unique_ptr<Identifier> function_name;
    std::vector<FormalParameter> parameters;
    std::unique_ptr<Block> body;
    CfgStatement* cfg_start = nullptr;
    CfgStatement* cfg_end = nullptr;

    struct MethodDeclarationObject *environment = nullptr;

    MethodDeclaration(
        std::vector<Modifier>& modifiers,
        std::unique_ptr<Type>& type,
        std::unique_ptr<Identifier>& function_name,
        std::vector<FormalParameter>& parameters,
        std::unique_ptr<Block>& body
    );
    MethodDeclaration(
        std::vector<Modifier>&& modifiers,
        std::unique_ptr<Type>&& type,
        std::unique_ptr<Identifier>&& function_name,
        std::vector<FormalParameter>&& parameters,
        std::unique_ptr<Block>&& body
    );
    MethodDeclaration(const MethodDeclaration &other) = default;
    MethodDeclaration(MethodDeclaration &&other) = default;
    ~MethodDeclaration();

    // Helper functions
    bool hasModifier(Modifier mod_to_check);
    std::string getSignature() const;

    // Operators
    friend bool operator==(const MethodDeclaration &lhs, const MethodDeclaration &rhs) {
        return (lhs.getSignature() == rhs.getSignature());
    }
    friend bool operator!=(const MethodDeclaration &lhs, const MethodDeclaration &rhs) { return !(lhs == rhs); }
};

struct ClassDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers; // Vector of class modifiers
    std::unique_ptr<Identifier> class_name; // Class name
    std::unique_ptr<QualifiedIdentifier> extends_class; // Class that this class extends off of
    std::vector<QualifiedIdentifier> implements; // Intefaces that this class implements
    std::vector<FieldDeclaration> field_declarations; // Class field declarations
    std::vector<MethodDeclaration> method_declarations; // Class method declarations

    struct ClassDeclarationObject *environment = nullptr;

    ClassDeclaration(
        std::vector<Modifier>& modifiers,
        std::unique_ptr<struct Identifier>& class_name,
        std::unique_ptr<QualifiedIdentifier>& extends_class,
        std::vector<QualifiedIdentifier>& implements,
        std::vector<FieldDeclaration>& field_declarations,
        std::vector<MethodDeclaration>& method_declarations
    );
    ClassDeclaration(
        std::vector<Modifier>&& modifiers,
        std::unique_ptr<struct Identifier>&& class_name,
        std::unique_ptr<QualifiedIdentifier>&& extends_class,
        std::vector<QualifiedIdentifier>&& implements,
        std::vector<FieldDeclaration>&& field_declarations,
        std::vector<MethodDeclaration>&& method_declarations
    );

    // Helper functions
    bool hasModifier(Modifier mod_to_check);
};

struct InterfaceDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers; // vector of interface modifiers
    std::unique_ptr<Identifier> interface_name; // interface name
    std::vector<QualifiedIdentifier> extends_class; // classes that this interface extends off of
    std::vector<MethodDeclaration> method_declarations; // interface method declarations

    struct InterfaceDeclarationObject *environment = nullptr;

    InterfaceDeclaration(
        std::vector<Modifier>& modifiers,
        std::unique_ptr<Identifier>& interface_name,
        std::vector<QualifiedIdentifier>& extends_class,
        std::vector<MethodDeclaration>& method_declarations
    );
    InterfaceDeclaration(
        std::vector<Modifier>&& modifiers,
        std::unique_ptr<Identifier>&& interface_name,
        std::vector<QualifiedIdentifier>&& extends_class,
        std::vector<MethodDeclaration>&& method_declarations
    );

    // Helper functions
    bool hasModifier(Modifier mod_to_check);
};
