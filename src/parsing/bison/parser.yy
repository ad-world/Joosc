%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.2"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

// Includes and forward declarations
%code requires {
  # include <string>
  # include <memory>
  # include <vector>
  # include "../parsetree/parsetreenode.h"
  # include "../parsetree/parsetreenode_t.h"
  class Driver;
}

// The parsing context
%param { Driver& driver }

%locations
%define parse.trace
%define parse.error verbose
%define parse.lac full

// Code inserted in generated .cc file
%code {
  # include "driver.h"
}

// TODO: not sure if this is necessary
%define api.token.prefix {TOK_}

/*=============================================================================
                              Token Definitions
=============================================================================*/
%token IF
%token WHILE
%token FOR
%token ELSE
%token EXTENDS
%token IMPLEMENTS
%token PUBLIC 
%token PROTECTED
%token PRIVATE
%token STATIC
%token ABSTRACT
%token THIS
%token VOID
%token FINAL
%token IMPORT
%token CLASS
%token NEW
%token INSTANCEOF

// might need to look at this again
%token PACKAGE
%token INTERFACE
%token OPENING_BRACE
%token CLOSING_BRACE
%token OPENING_PAREN
%token CLOSING_PAREN
%token OPENING_BRACKET
%token CLOSING_BRACKET
%token SEMI_COLON
%token COLON
%token COMMA
%token DOT
%token <std::string> IDENTIFIER
%token NATIVE
%token ASSIGNMENT
%token RETURN

// types
%token INT
%token BOOLEAN
%token CHAR 
%token BYTE
%token SHORT
%token ARRAY
%token TRUE
%token FALSE
%token STRING_LITERAL
%token INTEGER
%token NULL_TOKEN
%token CHAR_LITERAL

// comments
%token MULTI_LINE_COMMENT
%token SINGLE_LINE_COMMENT
%token JAVADOC_COMMENT

// operators
%token NEGATE
%token ASTERISK
%token PLUS
%token MINUS
%token DIVIDE
%token MODULO
%token LESS_THAN
%token GREATER_THAN
%token LESS_THAN_EQUAL
%token GREATER_THAN_EQUAL
%token BOOLEAN_EQUAL
%token NOT_EQUAL
%token AMPERSAND
%token PIPE
%token BOOLEAN_AND
%token BOOLEAN_OR

// REMOVE THESE
%token ResultType

// END OF FILE TOKEN
%token EOF 0
/*****************************************************************************/
%token VoidInterfaceMethodDeclaratorRest InterfaceMethodOrFieldDecl
%token MemberDecl InterfaceBodyDeclarations ClassInstanceCreationExpression

%token ClassBodyOpt

// Grammar
%%
%start CompilationUnit;

/*---------------------- Packages ----------------------*/

CompilationUnit:
    PackageDeclaration ImportDeclarations TypeDeclarations
    | ImportDeclarations TypeDeclarations   // No PackageDeclaration
    | PackageDeclaration TypeDeclarations   // No ImportDeclarations
    | PackageDeclaration ImportDeclarations // No TypeDeclarations
    |                                       // Empty
    ;

PackageDeclaration:
    PACKAGE QualifiedIdentifier SEMI_COLON
    ;

ImportDeclarations:
    ImportDeclaration
    | ImportDeclarations ImportDeclaration
    ;

TypeDeclarations:
    TypeDeclaration
    | TypeDeclarations TypeDeclaration
    ;

ImportDeclaration:
	SingleTypeImportDeclaration
	| TypeImportOnDemandDeclaration
    ;

SingleTypeImportDeclaration:
    IMPORT QualifiedIdentifier SEMI_COLON // TypeName
    ;

TypeImportOnDemandDeclaration:
    IMPORT QualifiedIdentifier DOT ASTERISK SEMI_COLON // PackageOrTypeName
    ;

TypeDeclaration:
    ClassDeclaration
    | InterfaceDeclaration
    | SEMI_COLON
    ;

/*---------------------- Expressions ----------------------*/

Expression:
    AssignmentExpression
    ;

AssignmentExpression:
    LeftHandSide ASSIGNMENT AssignmentExpression
    | ConditionalOrExpression
    ;

LeftHandSide:
    QualifiedIdentifier // ExpressionName
    | FieldAccess
    | ArrayAccess
    ;

ConditionalOrExpression:
    ConditionalAndExpression
    | ConditionalOrExpression BOOLEAN_OR ConditionalAndExpression
    ;

ConditionalAndExpression:
    EqualityExpression
    | ConditionalAndExpression BOOLEAN_AND EqualityExpression
    ;

EqualityExpression:
    RelationalExpression
    | EqualityExpression BOOLEAN_EQUAL RelationalExpression
    | EqualityExpression NOT_EQUAL RelationalExpression
    ;

RelationalExpression:
    AdditiveExpression
    | RelationalExpression LESS_THAN AdditiveExpression
    | RelationalExpression GREATER_THAN AdditiveExpression
    | RelationalExpression LESS_THAN_EQUAL AdditiveExpression
    | RelationalExpression GREATER_THAN_EQUAL AdditiveExpression
    | RelationalExpression INSTANCEOF ReferenceType // ReferenceType 
    ;

AdditiveExpression:
    MultiplicativeExpression
    | AdditiveExpression PLUS MultiplicativeExpression
    | AdditiveExpression MINUS MultiplicativeExpression
    ;

MultiplicativeExpression:
    UnaryExpression
    | MultiplicativeExpression ASTERISK UnaryExpression
    | MultiplicativeExpression DIVIDE UnaryExpression
    | MultiplicativeExpression MODULO UnaryExpression
    ;

UnaryExpression:
    MINUS UnaryExpression
    | UnaryExpressionNotPlusMinus
    ;

UnaryExpressionNotPlusMinus:
    NEGATE UnaryExpression
    | CastExpression
    | PrimaryOrExpressionName
    ;

CastExpression: // Done this way to avoid conflicts
    OPENING_PAREN PrimitiveType CLOSING_PAREN UnaryExpression
    | OPENING_PAREN Expression CLOSING_PAREN UnaryExpressionNotPlusMinus // Expression must be verified to be QualifiedIdentifier (ReferenceType no array)
    | OPENING_PAREN QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET CLOSING_PAREN UnaryExpressionNotPlusMinus // ReferenceType with array
    | OPENING_PAREN 
        PrimitiveType OPENING_BRACKET CLOSING_BRACKET // ReferenceType as PrimitiveType with array
            CLOSING_PAREN UnaryExpressionNotPlusMinus
    ;

PrimaryOrExpressionName:
    Primary
    | QualifiedIdentifier // ExpressionName
    ;

Primary:
    PrimaryNoNewArray
    | ArrayCreationExpression
    ;

ArrayCreationExpression:
    NEW PrimitiveType OPENING_BRACKET CLOSING_BRACKET
    | NEW PrimitiveType OPENING_BRACKET Expression CLOSING_BRACKET
    | NEW QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET // TypeName
    | NEW QualifiedIdentifier OPENING_BRACKET Expression CLOSING_BRACKET // TypeName
    ;

PrimaryNoNewArray:
    Literal
    | THIS
    | QualifiedIdentifier DOT THIS // ClassName
    | OPENING_PAREN Expression CLOSING_PAREN
    | ClassInstanceCreationExpression
    | FieldAccess
    | MethodInvocation
    | ArrayAccess
    ;

Literal:
    INTEGER
    | TRUE
    | FALSE
    | CHAR_LITERAL
    | STRING_LITERAL
    | NULL_TOKEN
    ;

ArrayAccess:
    QualifiedIdentifier OPENING_BRACKET Expression CLOSING_BRACKET // ExpressionName
    | PrimaryNoNewArray OPENING_BRACKET Expression CLOSING_BRACKET
    ;

FieldAccess:
    Primary DOT Identifier
    ;

ArgumentListOpt:
    /* Empty - No arguments */
    | ArgumentListOpt COMMA Expression
    ;

Arguments:
    OPENING_PAREN ArgumentListOpt CLOSING_PAREN
    ;

MethodInvocation:
    QualifiedIdentifier Arguments // MethodName
    | Primary DOT Identifier Arguments
    ;

Type:
    PrimitiveType
    | ReferenceType
    ;

PrimitiveType:
    IntegralType
    | BooleanType
    ;

IntegralType:
    BYTE
    | SHORT
    | INT
    ;

BooleanType:
    BOOLEAN
    ;

ClassOrInterfaceType:
    QualifiedIdentifier // ClassType, InterfaceType -> TypeName

ReferenceType: // Done this way to disallow multidimensional arrays
    ClassOrInterfaceType
    | ClassOrInterfaceType OPENING_BRACKET CLOSING_BRACKET
    | PrimitiveType OPENING_BRACKET CLOSING_BRACKET
    ;

/*---------------------- Interfaces ----------------------*/

InterfaceDeclaration:
    InterfaceModifiersOpt INTERFACE Identifier ExtendsInterfacesOpt InterfaceBody
    ;

InterfaceModifiers:
    InterfaceModifier 
    | InterfaceModifiers InterfaceModifier
    ;

InterfaceModifier:  
    PUBLIC
    | PROTECTED
    | PRIVATE
    | ABSTRACT
    | STATIC
    ;

InterfaceModifiersOpt:
    /* Empty - optional */
    | InterfaceModifiers
    ;

InterfaceType:
    QualifiedIdentifier
    ;

ExtendsInterfaces:
    EXTENDS InterfaceType
    | ExtendsInterfaces COMMA InterfaceType
    ;

ExtendsInterfacesOpt:
    /* Empty - optional interface */
    | ExtendsInterfaces
    ;

InterfaceBody:
    OPENING_BRACE InterfaceMemberDeclarationsOpt CLOSING_BRACE
    ;

InterfaceMemberDeclarationsOpt:
    /* Empty - No interface body declarations */
    | InterfaceMemberDeclarations
    ;

InterfaceMemberDeclarations:
    InterfaceMemberDeclaration
    | InterfaceMemberDeclarations InterfaceMemberDeclaration
    ;

InterfaceMemberDeclaration: // Nested types and interface constants not supported
    AbstractMethodDeclaration
    ;

AbstractMethodDeclaration:
    AbstractMethodModifiersOpt ResultType MethodDeclarator SEMI_COLON
    ;

AbstractMethodModifiersOpt:
    /* Empty - optional */
    | AbstractMethodModifiers
    ;

AbstractMethodModifiers:
    AbstractMethodModifier
    | AbstractMethodModifiers AbstractMethodModifier
    ;

AbstractMethodModifier:
    PUBLIC
    | ABSTRACT
    ;

MethodDeclarator: 
    Identifier OPENING_PAREN FormalParameterListOpt CLOSING_PAREN
    ;

FormalParameterListOpt:
    /* Empty - optional */
    | FormalParameterList
    ;

FormalParameterList:
	FormalParameter
	| FormalParameterList COMMA FormalParameter
    ;

FinalOpt:
    /* Empty - No final keyword */
    | FINAL
    ;

FormalParameter:
	FinalOpt Type VariableDeclaratorId
    ;

VariableDeclaratorId:
    Identifier
    | Identifier OPENING_BRACKET CLOSING_BRACKET
    ;

/*---------------------- Statements ----------------------*/

Statement:
    StatementWithoutTrailingSubstatement
    | IfThenStatement
    | IfThenElseStatement
    | WhileStatement
    | ForStatement
    ;

StatementWithoutTrailingSubstatement:
    Block
    | EmptyStatement
    | ExpressionStatement
    | ReturnStatement
    ;

ExpressionStatement:
    StatementExpression
    ;

StatementExpression:
    ASSIGNMENT
    | MethodInvocation
    | ClassInstanceCreationExpression
    ;

StatementNoShortIf:
    StatementWithoutTrailingSubstatement
    | IfThenElseStatementNoShortIf
    | WhileStatementNoShortIf
    | ForStatementNoShortIf
    ;

EmptyStatement:
    SEMI_COLON
	;

IfThenStatement:
	IF ParExpression Statement
    ;

IfThenElseStatement:
	IF ParExpression StatementNoShortIf ELSE Statement
    ;

IfThenElseStatementNoShortIf:
	IF ParExpression StatementNoShortIf ELSE StatementNoShortIf
    ;

WhileStatement:
	  WHILE ParExpression Statement
    ;

WhileStatementNoShortIf:
	  WHILE ParExpression StatementNoShortIf
    ;

ForStatement:
	  FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN Statement
    ;

ForStatementNoShortIf:
	  FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN StatementNoShortIf
    ;
		
ForInitOpt:
    /* No init */
    | ForInit
    ;

ForInit:
    StatementExpression
    | LocalVariableDeclaration
    ;

ForUpdateOpt:
    /* Empty - no update */
    | StatementExpression
    ;

ExpressionOpt:
    /* no expression */
    | Expression
    ;

ReturnStatement:
    RETURN ExpressionOpt SEMI_COLON
    ;

ParExpression: 
    OPENING_PAREN Expression CLOSING_PAREN
    ;

QualifiedIdentifier:
    Identifier
    | QualifiedIdentifier DOT Identifier
    ;

Identifier:
    IDENTIFIER
    ;

// Delay Type reduce due to conflict
LocalVariableDeclaration:
    // Type VariableDeclarators
    QualifiedIdentifier VariableDeclarators // ClassOrInterfaceType VariableDeclarators
    | QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET VariableDeclarators // ClassOrInterfaceTypeArray VariableDeclarators
    | PrimitiveType OPENING_BRACKET CLOSING_BRACKET VariableDeclarators // PrimitiveArray VariableDeclarators
    | PrimitiveType VariableDeclarators
    ;


// -------------------------------------------------------------

/* OLD CODE - CAN MODIFY/REMOVE */   


ModifiersOpt: 
    /* Empty - represents zero type declarations */
    | Modifier
    ;

Modifier:
    PUBLIC 
    | PROTECTED 
    | PRIVATE 
    | STATIC 
    | ABSTRACT 
    | FINAL 
    | NATIVE
    ;

ClassDeclaration:
    CLASS Identifier ExtendsOpt ImplementsOpt ClassBody
    ;

ExtendsOpt:
    /* Empty - represents zero type declarations */
    | EXTENDS Type
    ;

ImplementsOpt:
    /* Empty - represents zero type declarations */
    | IMPLEMENTS TypeList
    ;

ClassBody:
    OPENING_BRACE ClassBodyDeclarationsOpt CLOSING_BRACE
    ;

ClassBodyDeclarationsOpt:
    /* Empty - represents zero ClassBodyDeclarations */
    | ClassBodyDeclarationsOpt ClassBodyDeclaration
    ;

ClassBodyDeclaration:
    SEMI_COLON /* Empty declaration */
    | StaticOpt Block
    | ModifiersOpt MemberDecl
    ;

StaticOpt:
    /* Empty declaration */
    | STATIC
    ;

Block:
    OPENING_BRACE BlockStatementsOpt CLOSING_BRACE
    ;

BlockStatementsOpt:
    /* Empty - represents zero BlockStatements */
    | BlockStatements
    ;

BlockStatements:
    BlockStatement
    | BlockStatements BlockStatement
    ;

BlockStatement:
    LocalVariableDeclarationStatement 
    | ClassDeclaration
    | Statement
    ;

LocalVariableDeclarationStatement:
    LocalVariableDeclaration SEMI_COLON
    ;

VariableDeclarators:
    VariableDeclarator
    | VariableDeclarators COMMA VariableDeclarator
    ;

VariableDeclarator:
    VariableDeclaratorId
    | VariableDeclaratorId ASSIGNMENT VariableInitializer
    ;

VariableInitializer:
    ArrayInitializer
    | Expression
    ;

ArrayInitializer:
    OPENING_BRACE VariableInitializersOpt CLOSING_BRACE
    ;

VariableInitializersOpt:
    /* Empty - No variable initializers */
    | VariableInitializerList OptionalComma
    ;

VariableInitializerList:
    VariableInitializer
    | VariableInitializerList COMMA VariableInitializer
    ;

OptionalComma:
    /* Empty - No comma */
    | COMMA
    ;

TypeList:
    Type
    | TypeList COMMA Type
    ;
%%

void yy::parser::error (const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << '\n';
}
