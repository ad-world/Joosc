#pragma once

#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <string>

namespace Variant {

// The variant definition using all defined classes (this would be in its own file)
struct BinaryExpression;
struct UnaryExpression;
struct Integer;
struct CompilationUnit;

// Aryaman Work
struct QualifiedIdentifier;
struct ClassDeclaration;
struct InterfaceDeclaration;
struct Identifier;

enum Modifier {
    PUBLIC,
    PROTECTED,
    ABSTRACT,
    STATIC,
    NATIVE,
    FINAL
};

struct FieldDeclaration;
struct MethodDeclaration;
struct Type;
struct VariableDeclarator;
struct LocalVariableDeclaration;
struct IfThenStatement;
struct IfThenElseStatement;
struct WhileStatement;
struct ForStatement;
struct ReturnStatement;
struct Assignment;
struct MethodInvocation;
struct ClassInstanceCreationExpression;
struct Block;

typedef std::variant <
    Assignment,
    MethodInvocation,
    ClassInstanceCreationExpression
> ExpressionStatement;

typedef std::variant <
    // Statements
    IfThenStatement,
    IfThenElseStatement,
    WhileStatement,
    ForStatement, 
    Block,
    std::monostate, //EmptyStatement,
    ExpressionStatement,
    ReturnStatement,
    LocalVariableDeclaration
> Statement;

typedef std::variant<
    // Expressions
    BinaryExpression,
    UnaryExpression,
    Integer,
    // Statements
    ExpressionStatement,
    // Aryaman Work
    CompilationUnit,
    QualifiedIdentifier,
    ClassDeclaration,
    Identifier,
    FieldDeclaration,
    MethodDeclaration,
    Type,
    VariableDeclarator,
    Statement
> AstNode;

// For putting common decorator information e.g. Type
struct AstNodeCommon {
    virtual ~AstNodeCommon() = default;
};

struct BinaryExpression : public AstNodeCommon {
    enum BinaryOperator {
        PLUS, MINUS, TIMES, DIVIDE, 
    };
    BinaryOperator op;
    std::unique_ptr<AstNode> lhs;
    std::unique_ptr<AstNode> rhs;
};

struct UnaryExpression : public AstNodeCommon {
    enum UnaryOperator {
        NEGATE, // Boolean negate
        MINUS, // Number sign
    };
    UnaryOperator op;
    std::unique_ptr<AstNode> sub_expression;
};

struct Integer : public AstNodeCommon {
    int integer;
    Integer(int a) {this->integer = a;}
};

struct ExpressionStatement : public AstNodeCommon {
    std::unique_ptr<AstNode> statement_expression;
};

struct Identifier: public AstNodeCommon {
    std::string name; // Idenfier name

    Identifier(std::string& name) : name(std::move(name)) {}
};

struct QualifiedIdentifier: public AstNodeCommon {
    std::vector<Identifier> identifiers; // Vector of identifiers for this qualitfed identifier

    QualifiedIdentifier(std::vector<Identifier>& identifiers) : identifiers(std::move(identifiers)) {}
};


};