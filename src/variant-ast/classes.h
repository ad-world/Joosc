#pragma once

#include <memory>
#include <vector>
#include "astnodecommon.h"
#include "expressions.h"

struct Identifier;
struct QualifiedIdentifier;
struct Type;
struct Block;

enum class Modifier {
    PUBLIC,
    PROTECTED,
    ABSTRACT,
    STATIC,
    NATIVE,
    FINAL
};

struct VariableDeclarator {
    std::unique_ptr<Identifier> variable_name;
    std::unique_ptr<Expression> expression;

    VariableDeclarator(
        std::unique_ptr<Identifier> variable_name, 
        std::unique_ptr<Expression> expression
    );
};

struct FieldDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers;
    std::unique_ptr<Type> type;
    std::unique_ptr<VariableDeclarator> variable_declarator;

    FieldDeclaration(
        std::vector<Modifier> modifiers,
        std::unique_ptr<Type> type,
        std::unique_ptr<VariableDeclarator> variable_declarator
    );
};

struct FormalParameter {
    std::unique_ptr<Type> type;
    std::unique_ptr<Identifier> parameter_name;

    FormalParameter(
        std::unique_ptr<Type> type, 
        std::unique_ptr<Identifier> parameter_name
    );
};

struct MethodDeclaration {
    std::vector<Modifier> modifiers;
    std::unique_ptr<Type> type;
    std::unique_ptr<Identifier> function_name;
    std::vector<FormalParameter> parameters;
    std::unique_ptr<Block> body;

    MethodDeclaration(
        std::vector<Modifier> modifiers,
        std::unique_ptr<Type> type,
        std::unique_ptr<Identifier> function_name,
        std::vector<FormalParameter> parameters,
        std::unique_ptr<Block> body
    );
};

struct ClassDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers; // Vector of class modifiers
    std::unique_ptr<Identifier> class_name; // Class name
    std::unique_ptr<QualifiedIdentifier> extends_class; // Class that this class extends off of
    std::vector<QualifiedIdentifier> implements; // Intefaces that this class implements
    std::vector<FieldDeclaration> field_declarations; // Class field declarations
    std::vector<MethodDeclaration> method_declarations; // Class method declarations

    ClassDeclaration(
        std::vector<Modifier> modifiers,
        std::unique_ptr<struct Identifier> class_name,
        std::unique_ptr<QualifiedIdentifier> extends_class,
        std::vector<QualifiedIdentifier> implements,
        std::vector<FieldDeclaration> field_declarations,
        std::vector<MethodDeclaration> method_declarations
    );
};

struct InterfaceDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers; // vector of interface modifiers
    std::unique_ptr<Identifier> interface_name; // interface name
    std::unique_ptr<QualifiedIdentifier> extends_class; // class that this interface extends off of
    std::vector<MethodDeclaration> method_declarations; // interface method declarations

    InterfaceDeclaration(
        std::vector<Modifier> modifiers,
        std::unique_ptr<Identifier> interface_name,
        std::unique_ptr<QualifiedIdentifier> extends_class,
        std::vector<MethodDeclaration> method_declarations
    );
};
