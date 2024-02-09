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
struct ExpressionStatement;
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

struct CompilationUnit: public AstNodeCommon {
    QualifiedIdentifier package_declaration; // The singular package declaration
    std::vector<QualifiedIdentifier> single_type_import_declaration; // All single type imports
    std::vector<QualifiedIdentifier> type_import_on_demand_declaration; // All asterisk imports
    std::vector<ClassDeclaration> class_declarations; // All class declarations
    std::vector<InterfaceDeclaration> interface_declarations; // All import declarations

    CompilationUnit(
        QualifiedIdentifier& package_declaration, 
        std::vector<QualifiedIdentifier>& single_imports, 
        std::vector<QualifiedIdentifier>& asterisk_imports, 
        std::vector<ClassDeclaration>& class_decs, 
        std::vector<InterfaceDeclaration>& interface_decs
    ) : 
        package_declaration{std::move(package_declaration)},
        single_type_import_declaration{std::move(single_imports)},
        type_import_on_demand_declaration{std::move(asterisk_imports)},
        class_declarations{std::move(class_decs)},
        interface_declarations{std::move(interface_decs)}
    {}
};

struct ClassDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers; // Vector of class modifiers
    Identifier class_name; // Class name
    std::optional<QualifiedIdentifier> extends_class; // Class that this class extends off of
    std::vector<QualifiedIdentifier> implements; // Intefaces that this class implements
    std::vector<FieldDeclaration> field_declarations; // Class field declarations
    std::vector<MethodDeclaration> method_declarations; // Class method declarations

    ClassDeclaration(
        std::vector<Modifier>& modifiers,
        Identifier& class_name,
        std::optional<QualifiedIdentifier>& extends_class,
        std::vector<QualifiedIdentifier>& implements,
        std::vector<FieldDeclaration>& field_declarations,
        std::vector<MethodDeclaration>& method_declarations
    ) :
        modifiers{std::move(modifiers)},
        class_name{std::move(class_name)},
        extends_class{std::move(extends_class)},
        implements{std::move(implements)},
        field_declarations{std::move(field_declarations)},
        method_declarations{std::move(method_declarations)}
    {}
};

struct FieldDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers;
    // Type type;
    // VariableDeclarator variable_declarator;

    FieldDeclaration(
        std::vector<Modifier>& modifiers //,
        // Type& type,
        // VariableDeclarator& variable_declarator
    ) :
        modifiers(std::move(modifiers)) // ,
        // type(std::move(type)),
        // variable_declarator(std::move(variable_declarator))
    {}
};

struct LocalVariableDeclaration: public AstNodeCommon {
    // Type type;
    // VariableDeclarator variable_declarator;

    LocalVariableDeclaration(
        // Type& type,
        // VariableDeclarator& variable_declarator
    ) : 
        // type(std::move(type)),
        // variable_declarator(std::move(variable_declarator))
    {}
};

struct InterfaceDeclaration: public AstNodeCommon {
    std::vector<Modifier> modifiers; // vector of interface modifiers
    Identifier interface_name; // interface name
    std::optional<QualifiedIdentifier> extends_class; // class that this interface extends off of
    std::vector<MethodDeclaration> method_declarations; // interface method declarations

    InterfaceDeclaration(
        std::vector<Modifier>& modifiers,
        Identifier& interface_name,
        std::optional<QualifiedIdentifier>& extends_class,
        std::vector<MethodDeclaration>& method_declarations
    ) :
        modifiers{std::move(modifiers)},
        interface_name{std::move(interface_name)},
        extends_class{std::move(extends_class)},
        method_declarations{std::move(method_declarations)}
    {}
};

struct IfThenStatment: public AstNodeCommon {
    // Expression expression; // if clause
    Statement statement; // then clause

    IfThenStatment(
        // Expression& expression,
        Statement& statement
    ) : 
        // expression{std::move(expression)},
        statement{std::move(statement)}
    {}
};

struct IfThenElseStatment: public AstNodeCommon {
    // Expression expression; // if clause
    Statement statement_then; // then clause
    Statement statement_else; // else clause

    IfThenElseStatment(
        // Expression& expression,
        Statement& statement_then,
        Statement& statement_else
    ) : 
        // expression{std::move(expression)},
        statement_then{std::move(statement_then)},
        statement_else{std::move(statement_else)}
    {}
};

struct WhileStatement: public AstNodeCommon {
    // Expression expression; // condition clause
    Statement statement; // loop body

    WhileStatement(
        // Expression& expression,
        Statement& statement
    ) : 
        // expression{std::move(expression)},
        statement{std::move(statement)}
    {}
};

struct ForStatement: public AstNodeCommon {
    std::optional<Statement> statement_init; // ForInit
    // std::optional<Expression> expression; // condition clause
    std::optional<Statement> statement_update; // ForUpdate
    Statement statement_body; // Body

    ForStatement(
        std::optional<Statement>& statement_init,
        // std::optional<Expression>& expression,
        std::optional<Statement>& statement_update,
        Statement& statement_body
    ) : 
        statement_init{std::move(statement_init)},
        // expression{std::move(expression)},
        statement_update{std::move(statement_update)},
        statement_body{std::move(statement_body)}
    {}
};

struct ReturnStatement: public AstNodeCommon {
    // std::optional<Expression> expression;

    ReturnStatement(
        // std::optional<Expression>& expression
    ) : 
        // expression{std::move(expression)}
    {}
};


/*************************/

struct QualifiedIdentifier;
struct Identifier;
struct Expression;
struct FormalParameter;
struct Block;
struct LocalVariableDeclaration;
struct Statement;

enum PrimitiveType {
    BYTE, SHORT, INT, CHAR, BOOLEAN, VOID
};

typedef std::variant<PrimitiveType, QualifiedIdentifier> NonArrayType;

struct Type {
    std::unique_ptr<NonArrayType> non_array_type;
    bool is_array;

    Type(std::unique_ptr<NonArrayType>& non_array_type, bool is_array) :
        non_array_type{std::move(non_array_type)}, is_array{is_array} {}
};

struct VariableDeclarator {
    std::unique_ptr<Identifier> id;
    std::unique_ptr<Expression> expression;

    VariableDeclarator(std::unique_ptr<Identifier>& id, std::unique_ptr<Expression>& expression) :
        id{std::move(id)}, expression{std::move(expression)} {}
};

struct MethodDeclaration {
    std::vector<Modifier> modifiers;
    std::unique_ptr<Type> type;
    std::unique_ptr<Identifier> function_name;
    std::vector<FormalParameter> parameters;
    std::unique_ptr<Block> body;

    MethodDeclaration(
        std::vector<Modifier>& modifiers,
        std::unique_ptr<Type>& type,
        std::unique_ptr<Identifier>& function_name,
        std::vector<FormalParameter>& parameters,
        std::unique_ptr<Block>& body
    ) :
        modifiers{std::move(modifiers)}, 
        type{std::move(type)},
        function_name{std::move(function_name)},
        parameters{std::move(parameters)},
        body{std::move(body)} {}
};

struct Block {
    std::vector<LocalVariableDeclaration> variable_declarations;
    std::vector<Statement> statements;

    Block(
        std::vector<LocalVariableDeclaration>& variable_declarations, 
        std::vector<Statement>& statements
    ) :
        variable_declarations{std::move(variable_declarations)}, 
        statements{std::move(statements)} {}
};

struct FormalParameter {
    std::unique_ptr<Type> type;
    std::unique_ptr<Identifier> parameter_name;

    FormalParameter(
        std::unique_ptr<Type>& type, 
        std::unique_ptr<Identifier>& parameter_name
    ) :
        type{std::move(type)}, 
        parameter_name{std::move(parameter_name)} {}
};


};