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
%token PLUS
%token MINUS
%token ASTERISK
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

// END OF FILE TOKEN
%token EOF 0
/*****************************************************************************/
%token VoidInterfaceMethodDeclaratorRest InterfaceMethodOrFieldDecl Assignment
%token MemberDecl InterfaceBodyDeclarations ParExpression ReturnStatement ExpressionOpt LocalVariableDeclaration

%token ClassBodyOpt

// Grammar
%%
%start Type;

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

ClassInstanceCreationExpression:
    NEW ClassOrInterfaceType Arguments ClassBodyOpt
    | Primary DOT NEW Identifier Arguments ClassBodyOpt
    ;

/*---------------------- Types ----------------------*/
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

/*---------------------- ----------------------*/


/* OLD CODE - CAN MODIFY/REMOVE */   

CompilationUnit:
    PackageDeclarationOpt ImportDeclarationsOpt TypeDeclarationsOpt
    ;

PackageDeclarationOpt:
    /* Empty - no package declaration */
    | PACKAGE QualifiedIdentifier SEMI_COLON
    ;

ImportDeclarationsOpt:
    /* Empty - represents zero import declarations */
    | ImportDeclarationsOpt ImportDeclaration
    ;

TypeDeclarationsOpt:
    /* Empty - represents zero type declarations */
    | TypeDeclarationsOpt TypeDeclaration
    ;

QualifiedIdentifier:
    Identifier
    | QualifiedIdentifier DOT Identifier
    ;

Identifier:
    IDENTIFIER
    ;

ImportDeclaration:
    IMPORT Identifier DottedIdentifiers OptionalWildcard SEMI_COLON
    ;

DottedIdentifiers:
    /* Empty - no additional identifiers */
    | DottedIdentifiers DOT Identifier
    ;

OptionalWildcard:
    /* Empty - no wildcard */
    | DOT ASTERISK
    ;

TypeDeclaration:
    ClassOrInterfaceDeclaration
    | SEMI_COLON /* Empty */
    ;

ClassOrInterfaceDeclaration: 
    ModifiersOpt ClassDeclaration 
    | ModifiersOpt InterfaceDeclaration
    ;

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
    | BlockStatementsOpt BlockStatement
    ;

BlockStatement:
    LocalVariableDeclarationStatement
    | ClassOrInterfaceDeclaration
    | Statement
    ;

LocalVariableDeclarationStatement:
    FinalOpt Type VariableDeclarators SEMI_COLON
    ;

VariableDeclarators:
    VariableDeclarator
    | VariableDeclarators COMMA VariableDeclarator
    ;

VariableDeclarator:
    Identifier VariableDeclaratorRest
    ;

VariableDeclaratorRest:
    BracketsOpt VariableInitializerOpt
    ;

VariableInitializerOpt:
    /* Empty - No variable initializer */
    | ASSIGNMENT VariableInitializer
    ;

BracketsOpt:
    /* Empty - No brackets */
    | BracketsOpt OPENING_BRACKET CLOSING_BRACKET
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

// Expression: // Not sure about this
//     Expression3 ASSIGNMENT Expression1
//     | Expression3
//     ;

Expression1:
    Expression3
    | Expression3 InfixExpression
    | Expression3 InstanceofExpression
    ;

InfixExpression:
    Infixop Expression3
    | InfixExpression Infixop Expression3
    ;

InstanceofExpression:
    INSTANCEOF Type
    ;

Infixop:
    BOOLEAN_OR 
    | BOOLEAN_AND 
    | PIPE 
    | AMPERSAND 
    | BOOLEAN_EQUAL 
    | NOT_EQUAL 
    | LESS_THAN 
    | GREATER_THAN 
    | LESS_THAN_EQUAL 
    | GREATER_THAN_EQUAL 
    | PLUS 
    | MINUS 
    | ASTERISK 
    | DIVIDE 
    | MODULO 
    ;

Expression3:
    PrefixOp Expression3
    | NoPrefixExpression3
    ;

NoPrefixExpression3:
    OPENING_PAREN BasicType CLOSING_PAREN Expression3 // Primitive cast
    | OPENING_PAREN Expression CLOSING_PAREN NoPrefixExpression3 // Reference cast
    | Primary SelectorOpt
    ;

PrefixOp:
    NEGATE 
    | MINUS
    ;

SelectorOpt:
    /* Empty - No selector */
    | SelectorOpt Selector
    ;

// Primary:
//     OPENING_PAREN Expression CLOSING_PAREN
//     | THIS ArgumentsOpt
//     | Literal
//     | ClassInstanceCreationExpression
//     | Identifier // Following SelectorOpt can be equivalent to QualifiedIdentifier
//     | BasicType BracketsOpt DOT CLASS
//     ;

// ClassInstanceCreationExpression:
//     NEW Creator

Creator:
    QualifiedIdentifier ClassCreatorRest
    | QualifiedIdentifier ArrayCreatorRest
    | BasicType ArrayCreatorRest
    ;

ArrayCreatorRest:
    OPENING_BRACKET CLOSING_BRACKET ArrayInitializer
    | OPENING_BRACKET Expression CLOSING_BRACKET

Selector:
    DOT Identifier ArgumentsOpt
    | DOT THIS
    | DOT NEW InnerCreator
    | OPENING_BRACKET Expression CLOSING_BRACKET
    ;

ArgumentsOpt:
    /* Empty - No Arguments */
    | Arguments
    ;

// Arguments:
//     OPENING_PAREN ExpressionListOpt CLOSING_PAREN
//     ;

ExpressionListOpt:
    /* Empty - No Expressions */
    | ExpressionList
    ;

ExpressionList:
    Expression
    | ExpressionList COMMA Expression
    ;

InnerCreator:
    Identifier ClassCreatorRest
    ;

ClassCreatorRest:
    Arguments ClassBodyOpt
    ;

// /* ClassBodyOpt:
//     /* Empty - No ClassBody */
//     | ClassBody
//     ; */

FinalOpt:
    /* Empty - No final keyword */
    | FINAL
    ;

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
    Assignment
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
    StatementExpression
    ;

// Statement:
//   Block
//   | IF ParExpression Statement ElseOpt
//    | FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN Statement
//    | WHILE ParExpression Statement
//    | RETURN ExpressionOpt SEMI_COLON
//    | SEMI_COLON /* Empty statement */
//    | StatementExpression
//   ;

// ElseOpt:
//     /* Empty - No else part */
//     | ELSE Statement
//     ;

// ForInitOpt:
//     /* Empty - No initialization */
//     | ForInit
//     ;

// ForInit:
//     StatementExpression MoreStatementExpressions
//     | FinalOpt Type VariableDeclarators
//     ;

// MoreStatementExpressions:
//     /* Empty - No more expressions */
//     | MoreStatementExpressions COMMA StatementExpression
//     ;

// ForUpdateOpt:
//     /* Empty - No update */
//     | ForUpdate
//     ;

// ForUpdate:
//     StatementExpression MoreStatementExpressions
//     ;

// MemberDecl:
//     | MethodOrFieldDecl
//     | VOID Identifier MethodDeclaratorRest
//     | Identifier ConstructorDeclaratorRest
//     | ClassOrInterfaceDeclaration
//     ;

// MethodOrFieldDecl:
// 	  Type Identifier MethodOrFieldRest
//     ;

// MethodOrFieldRest:
//     VariableDeclaratorRest
//     MethodDeclaratorRest
//     ;

// Type:
//     QualifiedIdentifier BracketsOpt
//     | BasicType BracketsOpt
//     ;

BasicType:
    BYTE
    | SHORT
    | CHAR
    | INT
    | BOOLEAN
    ;

TypeList:
    Type
    | TypeList COMMA Type
    ;

ExpressionStatement:
    StatementExpression SEMI_COLON

// StatementExpression: // Expressions that can be used as statements when followed by semicolon
// 	Assignment
// 	MethodInvocation
// 	ClassInstanceCreationExpression
//     ;

// ParExpression: 
// 	  OPENING_PAREN Expression CLOSING_PAREN
//     ;

// ConstantDeclaratorsRest:
//     ConstantDeclaratorRest
//     | ConstantDeclaratorsRest COMMA ConstantDeclarator
//     ;

// ConstantDeclaratorRest:
//     BracketsOpt ASSIGNMENT VariableInitializer
//     ;

// ConstantDeclarator:
//     Identifier ConstantDeclaratorRest
//     ;

// VariableDeclaratorId: 
// 	  Identifier BracketsOpt
//     ;

// Literal:
//     INTEGER 	
//     | CHAR_LITERAL 	
//     | STRING_LITERAL 	
//     | TRUE
//     | FALSE
//     | NULL_TOKEN
//     ;

InterfaceDeclaration:
    INTERFACE Identifier ExtendsTypeListOpt InterfaceBody
    ;

ExtendsTypeListOpt:
    /* Empty - No extends clause */
    | EXTENDS TypeList
    ;

InterfaceBody:
    OPENING_BRACE InterfaceBodyDeclarationsOpt CLOSING_BRACE
    ;

InterfaceBodyDeclarationsOpt:
    /* Empty - No interface body declarations */
    | InterfaceBodyDeclarations
    ;

// InterfaceBodyDeclarations:
//     InterfaceBodyDeclaration
//     | InterfaceBodyDeclarations InterfaceBodyDeclaration
//     ;

// InterfaceBodyDeclaration:
//     SEMI_COLON 
//     | ModifiersOpt InterfaceMemberDecl
//     ;

// InterfaceMemberDecl:
//     InterfaceMethodOrFieldDecl
//     | VOID Identifier VoidInterfaceMethodDeclaratorRest
//     | ClassOrInterfaceDeclaration
//     ;

// InterfaceMethodOrFieldDecl:
// 	  Type Identifier InterfaceMethodOrFieldRest
//     ;

// InterfaceMethodOrFieldRest:
//     ConstantDeclaratorsRest SEMI_COLON
//     | InterfaceMethodDeclaratorRest
//     ;

// MethodDeclaratorRest:
//     FormalParameters BracketsOpt MethodBodyOrSemi
//     ;

// MethodBodyOrSemi:
//     MethodBody
//     | SEMI_COLON
//     ;

// InterfaceMethodDeclaratorRest:
// 	  FormalParameters BracketsOpt SEMI_COLON
//     ; 

// VoidInterfaceMethodDeclaratorRest:
// 	  FormalParameters SEMI_COLON
//     ; 

// ConstructorDeclaratorRest:
// 	  FormalParameters MethodBody
//     ;

// FormalParameters:
//     OPENING_PAREN FormalParameterListOpt CLOSING_PAREN
//     ;

// FormalParameterListOpt:
//     /* Empty - No formal parameters */
//     | FormalParameterList
//     ;

// FormalParameterList:
//     FormalParameter
//     | FormalParameterList COMMA FormalParameter
//     ;

// FormalParameter:
//     FinalOpt Type VariableDeclaratorId
//     ;

// MethodBody:
// 	  Block
//     ;
%%

void yy::parser::error (const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << '\n';
}
