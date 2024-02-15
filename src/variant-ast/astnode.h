#pragma once

#include <variant>
#include "packages.h"
#include "classes.h"
#include "expressions.h"
#include "statements.h"
#include "types.h"
#include "names.h"

// Include this header to include all AST classes

typedef std::variant<
    // Packages
    CompilationUnit,
    // Names
    QualifiedIdentifier,
    Identifier,
    // Types
    Type,
    NonArrayType,
    PrimitiveType,
    // Classes
    ClassDeclaration,
    InterfaceDeclaration,
    FieldDeclaration,
    MethodDeclaration,
    VariableDeclarator,
    FormalParameter,
    Modifier,
    // Statements
    Statement,
    ExpressionStatement,
    LocalVariableDeclaration,
    Block,
    IfThenStatement,
    IfThenElseStatement,
    WhileStatement,
    ForStatement,
    ReturnStatement,
    // Expressions
    Expression,
    InfixExpression,
    PrefixExpression,
    CastExpression,
    Assignment,
    QualifiedThis,
    ArrayCreationExpression,
    Literal,
    ClassInstanceCreationExpression,
    FieldAccess,
    ArrayAccess,
    MethodInvocation,
    InstanceOfExpression
> AstNodeVariant;
