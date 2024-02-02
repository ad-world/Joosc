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
  # include "../../ast/ast.h"

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
%token <AstNode*> IF
%token <AstNode*> WHILE
%token <AstNode*> FOR
%token <AstNode*> ELSE
%token <AstNode*> EXTENDS
%token <AstNode*> IMPLEMENTS
%token <AstNode*> PUBLIC
%token <AstNode*> PROTECTED
%token <AstNode*> PRIVATE
%token <AstNode*> STATIC
%token <AstNode*> ABSTRACT
%token <AstNode*> THIS
%token <AstNode*> VOID
%token <AstNode*> FINAL
%token <AstNode*> IMPORT
%token <AstNode*> CLASS
%token <AstNode*> NEW
%token <AstNode*> INSTANCEOF

// might need to look at this again
%token <AstNode*> PACKAGE
%token <AstNode*> INTERFACE
%token <AstNode*> OPENING_BRACE
%token <AstNode*> CLOSING_BRACE
%token <AstNode*> OPENING_PAREN
%token <AstNode*> CLOSING_PAREN
%token <AstNode*> OPENING_BRACKET
%token <AstNode*> CLOSING_BRACKET
%token <AstNode*> SEMI_COLON
%token <AstNode*> COLON
%token <AstNode*> COMMA
%token <AstNode*> DOT
%token <AstNode*> IDENTIFIER
%token <AstNode*> NATIVE
%token <AstNode*> ASSIGNMENT
%token <AstNode*> RETURN

// types
%token <AstNode*> INT
%token <AstNode*> BOOLEAN
%token <AstNode*> CHAR
%token <AstNode*> BYTE
%token <AstNode*> SHORT
%token <AstNode*> TRUE
%token <AstNode*> FALSE
%token <AstNode*> STRING_LITERAL
%token <AstNode*> INTEGER
%token <AstNode*> NULL_TOKEN
%token <AstNode*> CHAR_LITERAL

// comments
%token <AstNode*> MULTI_LINE_COMMENT
%token <AstNode*> SINGLE_LINE_COMMENT
%token <AstNode*> JAVADOC_COMMENT

// operators
%token <AstNode*> NEGATE
%token <AstNode*> ASTERISK
%token <AstNode*> PLUS
%token <AstNode*> MINUS
%token <AstNode*> DIVIDE
%token <AstNode*> MODULO
%token <AstNode*> LESS_THAN
%token <AstNode*> GREATER_THAN
%token <AstNode*> LESS_THAN_EQUAL
%token <AstNode*> GREATER_THAN_EQUAL
%token <AstNode*> BOOLEAN_EQUAL
%token <AstNode*> NOT_EQUAL
%token <AstNode*> AMPERSAND
%token <AstNode*> PIPE
%token <AstNode*> BOOLEAN_AND
%token <AstNode*> BOOLEAN_OR

// END OF FILE TOKEN
%token <AstNode*> EOF 0
/*****************************************************************************/

/************************* NONTERMINALS *************************/
%nterm <AstNode*> CompilationUnit
%nterm <AstNode*> PackageDeclaration
%nterm <AstNode*> ImportDeclarations
%nterm <AstNode*> TypeDeclarations
%nterm <AstNode*> ImportDeclaration
%nterm <AstNode*> SingleTypeImportDeclaration
%nterm <AstNode*> TypeImportOnDemandDeclaration
%nterm <AstNode*> TypeDeclaration
%nterm <AstNode*> Expression
%nterm <AstNode*> AssignmentExpression
%nterm <AstNode*> Assignment
%nterm <AstNode*> LeftHandSide
%nterm <AstNode*> ConditionalOrExpression
%nterm <AstNode*> ConditionalAndExpression
%nterm <AstNode*> EqualityExpression
%nterm <AstNode*> RelationalExpression
%nterm <AstNode*> AdditiveExpression
%nterm <AstNode*> MultiplicativeExpression
%nterm <AstNode*> UnaryExpression
%nterm <AstNode*> UnaryExpressionNotPlusMinus
%nterm <AstNode*> CastExpression
%nterm <AstNode*> PrimaryOrExpressionName
%nterm <AstNode*> Primary
%nterm <AstNode*> ArrayCreationExpression
%nterm <AstNode*> ClassInstanceCreationExpression
%nterm <AstNode*> PrimaryNoNewArray
%nterm <AstNode*> Literal
%nterm <AstNode*> ArrayAccess
%nterm <AstNode*> FieldAccess
%nterm <AstNode*> ArgumentListOpt
%nterm <AstNode*> ArgumentList
%nterm <AstNode*> Arguments
%nterm <AstNode*> MethodInvocation
%nterm <AstNode*> Type
%nterm <AstNode*> PrimitiveType
%nterm <AstNode*> IntegralType
%nterm <AstNode*> BooleanType
%nterm <AstNode*> ClassOrInterfaceType
%nterm <AstNode*> ReferenceType
%nterm <AstNode*> InterfaceDeclaration
%nterm <AstNode*> Modifiers
%nterm <AstNode*> Modifier
%nterm <AstNode*> InterfaceModifiersOpt
%nterm <AstNode*> InterfaceType
%nterm <AstNode*> ExtendsInterfaces
%nterm <AstNode*> ExtendsInterfacesOpt
%nterm <AstNode*> InterfaceBody
%nterm <AstNode*> InterfaceMemberDeclarationsOpt
%nterm <AstNode*> InterfaceMemberDeclarations
%nterm <AstNode*> InterfaceMemberDeclaration
%nterm <AstNode*> AbstractMethodDeclaration
%nterm <AstNode*> AbstractMethodModifiersOpt
%nterm <AstNode*> AbstractMethodModifiers
%nterm <AstNode*> ClassDeclaration
%nterm <AstNode*> InterfacesOpt
%nterm <AstNode*> Interfaces
%nterm <AstNode*> InterfaceTypeList
%nterm <AstNode*> ExtendsOpt
%nterm <AstNode*> ClassBodyDeclarationsOpt
%nterm <AstNode*> ClassBodyDeclarations
%nterm <AstNode*> ClassBodyDeclaration
%nterm <AstNode*> ClassMemberDeclaration
%nterm <AstNode*> ClassBody
%nterm <AstNode*> FieldDeclaration
%nterm <AstNode*> VariableInitializer
%nterm <AstNode*> MethodDeclaration
%nterm <AstNode*> MethodHeader
%nterm <AstNode*> MethodDeclarator
%nterm <AstNode*> MethodBody
%nterm <AstNode*> FormalParameterListOpt
%nterm <AstNode*> FormalParameterList
%nterm <AstNode*> AbstractMethodModifier
%nterm <AstNode*> FinalOpt
%nterm <AstNode*> FormalParameter
%nterm <AstNode*> VariableDeclaratorId
%nterm <AstNode*> Statement
%nterm <AstNode*> StatementWithoutTrailingSubstatement
%nterm <AstNode*> ExpressionStatement
%nterm <AstNode*> StatementExpression
%nterm <AstNode*> StatementNoShortIf
%nterm <AstNode*> EmptyStatement
%nterm <AstNode*> IfThenStatement
%nterm <AstNode*> IfThenElseStatement
%nterm <AstNode*> IfThenElseStatementNoShortIf
%nterm <AstNode*> WhileStatement
%nterm <AstNode*> WhileStatementNoShortIf
%nterm <AstNode*> ForStatement
%nterm <AstNode*> ForStatementNoShortIf
%nterm <AstNode*> ForInitOpt
%nterm <AstNode*> ForInit
%nterm <AstNode*> ForUpdateOpt
%nterm <AstNode*> ExpressionOpt
%nterm <AstNode*> ReturnStatement
%nterm <AstNode*> ParExpression
%nterm <AstNode*> QualifiedIdentifier
%nterm <AstNode*> Identifier
%nterm <AstNode*> LocalVariableDeclaration
%nterm <AstNode*> Block
%nterm <AstNode*> BlockStatementsOpt
%nterm <AstNode*> BlockStatements
%nterm <AstNode*> BlockStatement
%nterm <AstNode*> LocalVariableDeclarationStatement
%nterm <AstNode*> VariableDeclarator
/******************** END NONTERMINALS ********************/

%parse-param {AstNode **root}

%{
#define ASSIGN_SYMBOL(symbol)           $$ = new AstNode((symbol))

#define MAKE_EMPTY()                    ASSIGN_SYMBOL(symbol_kind::S_YYEMPTY)
#define MAKE_NODE(symbol, children...)  ASSIGN_SYMBOL(symbol); $$.addChild(children)

#define MAKE_1  ()              $$ = $1
#define MAKE_2  (symbol)        MAKE_NODE(symbol, $1, $2)
#define MAKE_3  (symbol)        MAKE_NODE(symbol, $1, $2, $3)
#define MAKE_4  (symbol)        MAKE_NODE(symbol, $1, $2, $3, $4)
#define MAKE_5  (symbol)        MAKE_NODE(symbol, $1, $2, $3, $4, $5)
#define MAKE_6  (symbol)        MAKE_NODE(symbol, $1, $2, $3, $4, $5, $6)
#define MAKE_7  (symbol)        MAKE_NODE(symbol, $1, $2, $3, $4, $5, $6, $7)
#define MAKE_8  (symbol)        MAKE_NODE(symbol, $1, $2, $3, $4, $5, $6, $7, $8)
#define MAKE_9  (symbol)        MAKE_NODE(symbol, $1, $2, $3, $4, $5, $6, $7, $8, $9)
%}

// Grammar
%%
%start CompilationUnit;

/*---------------------- Packages ----------------------*/

CompilationUnit:
    PackageDeclaration ImportDeclarations TypeDeclarations
        { MAKE_3(symbol_kind::S_CompilationUnit); }
    | ImportDeclarations TypeDeclarations   // No PackageDeclaration
        { MAKE_2(symbol_kind::S_CompilationUnit); }
    | PackageDeclaration TypeDeclarations   // No ImportDeclarations
        { MAKE_2(symbol_kind::S_CompilationUnit); }
    | PackageDeclaration ImportDeclarations // No TypeDeclarations
        { MAKE_2(symbol_kind::S_CompilationUnit); }
    | PackageDeclaration { MAKE_1(); }
    | ImportDeclaration { MAKE_1(); }
    | TypeDeclaration { MAKE_1(); }
    | /* Empty */ { MAKE_EMPTY(); }
    ;

PackageDeclaration:
    PACKAGE QualifiedIdentifier SEMI_COLON
        { MAKE_3(symbol_kind::S_PackageDeclaration); }
    ;

ImportDeclarations:
    ImportDeclaration { MAKE_1(); }
    | ImportDeclarations ImportDeclaration
        { MAKE_2(symbol_kind::S_ImportDeclarations); }
    ;

TypeDeclarations:
    TypeDeclaration { MAKE_1(); }
    | TypeDeclarations TypeDeclaration { MAKE_2(symbol_kind::S_TypeDeclarations); }
    ;

ImportDeclaration:
	SingleTypeImportDeclaration { MAKE_1(); }
	| TypeImportOnDemandDeclaration { MAKE_1(); }
    ;

SingleTypeImportDeclaration:
    IMPORT QualifiedIdentifier SEMI_COLON // TypeName
        { MAKE_3(symbol_kind::S_SingleTypeImportDeclaration); }
    ;

TypeImportOnDemandDeclaration:
    IMPORT QualifiedIdentifier DOT ASTERISK SEMI_COLON // PackageOrTypeName
        { MAKE_5(symbol_kind::S_TypeImportOnDemandDeclaration); }
    ;

TypeDeclaration:
    ClassDeclaration { MAKE_1(); }
    | InterfaceDeclaration { MAKE_1(); }
    | SEMI_COLON { MAKE_1(); }
    ;

/*---------------------- Expressions ----------------------*/

Expression:
    AssignmentExpression { MAKE_1(); }
    ;

AssignmentExpression:
    Assignment { MAKE_1(); }
    | ConditionalOrExpression { MAKE_1(); }
    ;

Assignment:
    LeftHandSide ASSIGNMENT AssignmentExpression { MAKE_3(symbol_kind::S_Assignment); }

LeftHandSide:
    QualifiedIdentifier // ExpressionName
        { MAKE_1(); }
    | FieldAccess { MAKE_1(); }
    | ArrayAccess { MAKE_1(); }
    ;

ConditionalOrExpression:
    ConditionalAndExpression { MAKE_1(); }
    | ConditionalOrExpression BOOLEAN_OR ConditionalAndExpression
        { MAKE_3(symbol_kind::S_ConditionalOrExpression); }
    ;

ConditionalAndExpression:
    EqualityExpression { MAKE_1(); }
    | ConditionalAndExpression BOOLEAN_AND EqualityExpression
        { MAKE_3(symbol_kind::S_ConditionalAndExpression); }
    ;

EqualityExpression:
    RelationalExpression { MAKE_1(); }
    | EqualityExpression BOOLEAN_EQUAL RelationalExpression  { MAKE_3(symbol_kind::S_EqualityExpression); }
    | EqualityExpression NOT_EQUAL RelationalExpression { MAKE_3(symbol_kind::S_EqualityExpression); }
    ;

RelationalExpression:
    AdditiveExpression { MAKE_1(); }
    | RelationalExpression LESS_THAN AdditiveExpression
        { MAKE_3(symbol_kind::S_RelationalExpression); }
    | RelationalExpression GREATER_THAN AdditiveExpression
        { MAKE_3(symbol_kind::S_RelationalExpression); }
    | RelationalExpression LESS_THAN_EQUAL AdditiveExpression
        { MAKE_3(symbol_kind::S_RelationalExpression); }
    | RelationalExpression GREATER_THAN_EQUAL AdditiveExpression
        { MAKE_3(symbol_kind::S_RelationalExpression); }
    | RelationalExpression INSTANCEOF ReferenceType // ReferenceType
        { MAKE_3(symbol_kind::S_RelationalExpression); }
    ;

AdditiveExpression:
    MultiplicativeExpression { MAKE_1(); }
    | AdditiveExpression PLUS MultiplicativeExpression { MAKE_3(symbol_kind::S_AdditiveExpression); }
    | AdditiveExpression MINUS MultiplicativeExpression { MAKE_3(symbol_kind::S_AdditiveExpression); }
    ;

MultiplicativeExpression:
    UnaryExpression { MAKE_1(); }
    | MultiplicativeExpression ASTERISK UnaryExpression { MAKE_3(symbol_kind::S_MultiplicativeExpression); }
    | MultiplicativeExpression DIVIDE UnaryExpression { MAKE_3(symbol_kind::S_MultiplicativeExpression); }
    | MultiplicativeExpression MODULO UnaryExpression { MAKE_3(symbol_kind::S_MultiplicativeExpression); }
    ;

UnaryExpression:
    MINUS UnaryExpression { MAKE_2(symbol_kind::S_UnaryExpression); }
    | UnaryExpressionNotPlusMinus { MAKE_1(); }
    ;

UnaryExpressionNotPlusMinus:
    NEGATE UnaryExpression { MAKE_2(symbol_kind::S_UnaryExpressionNotPlusMinus); }
    | CastExpression { MAKE_1(); }
    | PrimaryOrExpressionName { MAKE_1(); }
    ;

CastExpression: // Done this way to avoid conflicts
    OPENING_PAREN PrimitiveType CLOSING_PAREN UnaryExpression
        { MAKE_4(symbol_kind::S_CastExpression); }
    | OPENING_PAREN Expression CLOSING_PAREN UnaryExpressionNotPlusMinus // Expression must be verified to be QualifiedIdentifier (ReferenceType no array)
        { MAKE_4(symbol_kind::S_CastExpression); }
    | OPENING_PAREN QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET CLOSING_PAREN UnaryExpressionNotPlusMinus // ReferenceType with array
        { MAKE_6(symbol_kind::S_CastExpression); }
    | OPENING_PAREN
        PrimitiveType OPENING_BRACKET CLOSING_BRACKET // ReferenceType as PrimitiveType with array
            CLOSING_PAREN UnaryExpressionNotPlusMinus
        { MAKE_6(symbol_kind::S_CastExpression); }
    ;

PrimaryOrExpressionName:
    Primary { MAKE_1(); }
    | QualifiedIdentifier // ExpressionName
        { MAKE_1(); }
    ;

Primary:
    PrimaryNoNewArray { MAKE_1(); }
    | ArrayCreationExpression { MAKE_1(); }
    ;

ArrayCreationExpression:
    NEW PrimitiveType OPENING_BRACKET CLOSING_BRACKET
        { MAKE_4(symbol_kind::S_ArrayCreationExpression); }
    | NEW PrimitiveType OPENING_BRACKET Expression CLOSING_BRACKET
        { MAKE_5(symbol_kind::S_ArrayCreationExpression); }
    | NEW QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET // TypeName
        { MAKE_4(symbol_kind::S_ArrayCreationExpression); }
    | NEW QualifiedIdentifier OPENING_BRACKET Expression CLOSING_BRACKET // TypeName
        { MAKE_5(symbol_kind::S_ArrayCreationExpression); }
    ;

ClassInstanceCreationExpression:
    NEW QualifiedIdentifier OPENING_PAREN ArgumentListOpt CLOSING_PAREN
        { MAKE_5(symbol_kind::S_ClassInstanceCreationExpression); }

PrimaryNoNewArray:
    Literal { MAKE_1(); }
    | THIS { MAKE_1(); }
    | QualifiedIdentifier DOT THIS // ClassName
        { MAKE_3(symbol_kind::S_PrimaryNoNewArray); }
    | OPENING_PAREN Expression CLOSING_PAREN
        { MAKE_3(symbol_kind::S_PrimaryNoNewArray); }
    | ClassInstanceCreationExpression { MAKE_1(); }
    | FieldAccess { MAKE_1(); }
    | MethodInvocation { MAKE_1(); }
    | ArrayAccess { MAKE_1(); }
    ;

Literal:
    INTEGER  { MAKE_1(); }
    | TRUE  { MAKE_1(); }
    | FALSE  { MAKE_1(); }
    | CHAR_LITERAL  { MAKE_1(); }
    | STRING_LITERAL  { MAKE_1(); }
    | NULL_TOKEN   { MAKE_1(); }
    ;

ArrayAccess:
    QualifiedIdentifier OPENING_BRACKET Expression CLOSING_BRACKET // ExpressionName
        { MAKE_4(symbol_kind::S_ArrayAccess); }
    | PrimaryNoNewArray OPENING_BRACKET Expression CLOSING_BRACKET
        { MAKE_4(symbol_kind::S_ArrayAccess); }
    ;

FieldAccess:
    Primary DOT Identifier
        { MAKE_3(symbol_kind::S_FieldAccess); }
    ;

ArgumentListOpt:
    /* Empty - No arguments */ { MAKE_EMPTY(); }
    | ArgumentList { MAKE_1(); }
    ;

ArgumentList:
    Expression { MAKE_1(); }
    | Expression COMMA ArgumentList
        { MAKE_3(symbol_kind::S_ArgumentList); }
    ;

Arguments:
    OPENING_PAREN ArgumentListOpt CLOSING_PAREN
        { MAKE_3(symbol_kind::S_Arguments); }
    ;

MethodInvocation:
    QualifiedIdentifier Arguments // MethodName
        { MAKE_2(symbol_kind::S_MethodInvocation); }
    | Primary DOT Identifier Arguments
        { MAKE_4(symbol_kind::S_MethodInvocation); }
    ;

Type:
    PrimitiveType { MAKE_1(); }
    | ReferenceType { MAKE_1(); }
    ;

PrimitiveType:
    IntegralType { MAKE_1(); }
    | BooleanType { MAKE_1(); }
    ;

IntegralType:
    BYTE { MAKE_1(); }
    | SHORT { MAKE_1(); }
    | INT { MAKE_1(); }
    ;

BooleanType:
    BOOLEAN { MAKE_1(); }
    ;

ClassOrInterfaceType:
    QualifiedIdentifier // ClassType, InterfaceType -> TypeName
        { MAKE_1(); }

ReferenceType: // Done this way to disallow multidimensional arrays
    ClassOrInterfaceType
        { MAKE_1(); }
    | ClassOrInterfaceType OPENING_BRACKET CLOSING_BRACKET
        { MAKE_3(symbol_kind::S_ReferenceType); }
    | PrimitiveType OPENING_BRACKET CLOSING_BRACKET
        { MAKE_3(symbol_kind::S_ReferenceType); }
    ;

/*---------------------- Interfaces ----------------------*/

InterfaceDeclaration:
    InterfaceModifiersOpt INTERFACE Identifier ExtendsInterfacesOpt InterfaceBody
        { MAKE_5(symbol_kind::S_InterfaceDeclaration); }
    ;

Modifiers:
    Modifier { MAKE_1(); }
    | Modifiers Modifier { MAKE_2(symbol_kind::S_Modifiers); }
    ;

Modifier:
    PUBLIC  { MAKE_1(); }
    | PROTECTED  { MAKE_1(); }
    | PRIVATE  { MAKE_1(); }
    | ABSTRACT  { MAKE_1(); }
    | STATIC  { MAKE_1(); }
    | NATIVE  { MAKE_1(); }
    ;

InterfaceModifiersOpt:
    /* Empty - optional */ { MAKE_EMPTY(); }
    | Modifiers { MAKE_1(); }
    ;

InterfaceType:
    QualifiedIdentifier { MAKE_1(); }
    ;

ExtendsInterfaces:
    EXTENDS InterfaceType { MAKE_2(symbol_kind::S_ExtendsInterfaces); }
    | ExtendsInterfaces COMMA InterfaceType { MAKE_3(symbol_kind::S_ExtendsInterfaces); }
    ;

ExtendsInterfacesOpt:
    /* Empty - optional interface */ { MAKE_EMPTY(); }
    | ExtendsInterfaces { MAKE_1(); }
    ;

InterfaceBody:
    OPENING_BRACE InterfaceMemberDeclarationsOpt CLOSING_BRACE
        { MAKE_3(symbol_kind::S_InterfaceBody); }
    ;

InterfaceMemberDeclarationsOpt:
    /* Empty - No interface body declarations */ { MAKE_EMPTY(); }
    | InterfaceMemberDeclarations { MAKE_1(); }
    ;

InterfaceMemberDeclarations:
    InterfaceMemberDeclaration { MAKE_1(); }
    | InterfaceMemberDeclarations InterfaceMemberDeclaration
        { MAKE_2(symbol_kind::S_InterfaceMemberDeclarations); }
    ;

InterfaceMemberDeclaration: // Nested types and interface constants not supported
    AbstractMethodDeclaration { MAKE_1(); }
    ;

AbstractMethodDeclaration:
    AbstractMethodModifiersOpt Type MethodDeclarator SEMI_COLON
        { MAKE_4(symbol_kind::S_AbstractMethodDeclaration); }
    | AbstractMethodModifiersOpt VOID MethodDeclarator SEMI_COLON
        { MAKE_4(symbol_kind::S_AbstractMethodDeclaration); }
    ;

AbstractMethodModifiersOpt:
    /* Empty - optional */ { MAKE_EMPTY(); }
    | AbstractMethodModifiers { MAKE_1(); }
    ;

AbstractMethodModifiers:
    AbstractMethodModifier
        { MAKE_1(); }
    | AbstractMethodModifiers AbstractMethodModifier
        { MAKE_2(symbol_kind::S_AbstractMethodModifiers); }
    ;


/*---------------------- Classes ---------------------- JAGVIR START HERE */

// weeder: make sure at most one of implements and extends is present
// weeder: modifier can only be abstract, final, public
// weeder: max one of abstract/final
// weeder: must contain public?
ClassDeclaration:
    CLASS Identifier ExtendsOpt InterfacesOpt ClassBody
        { MAKE_FIVE(symbol_kind::S_ClassDeclaration); }
    | Modifiers CLASS Identifier ExtendsOpt InterfacesOpt ClassBody
        { MAKE_SIX(symbol_kind::S_ClassDeclaration); }
    ;

/* Class interfaces */
InterfacesOpt:
    /* Empty - No implements */ { MAKE_EMPTY(); }
    | Interfaces { MAKE_1(); }
    ;

Interfaces:
    IMPLEMENTS InterfaceTypeList { MAKE_2(symbol_kind::S_Interfaces); }
    ;

InterfaceTypeList:
    InterfaceType { MAKE_1(); }
    | InterfaceTypeList COMMA InterfaceType { MAKE_3(symbol_kind::S_InterfaceTypeList); }
    ;

/* Class Extends */
ExtendsOpt:
    /* Empty - No extends */ { MAKE_EMPTY(); }
    | EXTENDS QualifiedIdentifier { MAKE_2(symbol_kind::S_ExtendsOpt); } // ClassType
    ;

/* Class body */
ClassBodyDeclarationsOpt:
    /* Empty */ { MAKE_EMPTY(); }
    | ClassBodyDeclarations { MAKE_1(); }
    ;

ClassBodyDeclarations:
    ClassBodyDeclaration { MAKE_EMPTY(); }
    | ClassBodyDeclarations ClassBodyDeclaration { MAKE_2(symbol_kind::S_ClassBodyDeclarations); }
    ;

ClassBodyDeclaration:
    ClassMemberDeclaration { MAKE_1(); }
    ;
    // ensure there is at least one constructor
    // | InstanceInitializer: omitted from joos
    // | StaticInitializer: omitted from joos

ClassMemberDeclaration:
    FieldDeclaration { MAKE_1(); }
    | MethodDeclaration { MAKE_1(); }
    | SEMI_COLON { MAKE_1(); }
    ;
    // | ClassDeclaration: omitted for joos
    // | InterfaceDeclaration: NOT SURE IF THIS SHOULD BE OMITTED FOR JOOS

ClassBody:
    OPENING_BRACE ClassBodyDeclarationsOpt CLOSING_BRACE { MAKE_3(symbol_kind::S_ClassBody); }
    ;

/* Fields */
// Only one variable declaration is allowed at a time
FieldDeclaration:
    Type VariableDeclarator SEMI_COLON { MAKE_3(symbol_kind::S_FieldDeclaration); }
    | Modifiers Type VariableDeclarator SEMI_COLON { MAKE_4(symbol_kind::S_FieldDeclaration); }
    ;

VariableInitializer:
    Expression { MAKE_1(); }
    ;

/* Methods */
// weeder: methodbody exists if neither abstract nor native
MethodDeclaration: // One of these must be constructor
    MethodHeader MethodBody { MAKE_2(symbol_kind::S_MethodDeclaration); }
    ;

// weeder: allow static native int m(int)
// weeder: see A1 specs for weeding modifiers
MethodHeader:
    Type MethodDeclarator { MAKE_2(symbol_kind::S_MethodHeader); }
    | Modifiers Type MethodDeclarator { MAKE_3(symbol_kind::S_MethodHeader); }
    | VOID MethodDeclarator { MAKE_2(symbol_kind::S_MethodHeader); }
    | Modifiers VOID MethodDeclarator { MAKE_3(symbol_kind::S_MethodHeader); }
    | Modifiers MethodDeclarator // Represents constructor, todo weeding: reject if identifier is not class name
        { MAKE_2(symbol_kind::S_MethodHeader); }
    ;

MethodDeclarator:
    Identifier OPENING_PAREN FormalParameterListOpt CLOSING_PAREN
        { MAKE_4(symbol_kind::S_MethodDeclarator); }
    ;

MethodBody:
    SEMI_COLON { MAKE_1(); }
    | Block { MAKE_1(); }
    ;

/* Formal parameters */
FormalParameterListOpt:
    { MAKE_EMPTY(); }
    | FormalParameterList { MAKE_1(); }
    ;

FormalParameterList:
    FormalParameter { MAKE_1(); }
    | FormalParameterList COMMA FormalParameter { MAKE_3(symbol_kind::S_FormalParameterList); }
    ;
/*-----------------------*/

AbstractMethodModifier:
    PUBLIC { MAKE_1(); }
    | ABSTRACT { MAKE_1(); }
    ;

FinalOpt:
    /* Empty - No final keyword */ { MAKE_EMPTY(); }
    | FINAL { MAKE_1(); }
    ;

FormalParameter:
	FinalOpt Type VariableDeclaratorId { MAKE_3(symbol_kind::S_FormalParameter); }
    ;

VariableDeclaratorId:
    Identifier { MAKE_1(); }
    | Identifier OPENING_BRACKET CLOSING_BRACKET { MAKE_3(symbol_kind::S_VariableDeclaratorId); }
    ;

/*---------------------- Statements ----------------------*/

Statement:
    StatementWithoutTrailingSubstatement { MAKE_1(); }
    | IfThenStatement { MAKE_1(); }
    | IfThenElseStatement { MAKE_1(); }
    | WhileStatement { MAKE_1(); }
    | ForStatement { MAKE_1(); }
    ;

StatementWithoutTrailingSubstatement:
    Block { MAKE_1(); }
    | EmptyStatement { MAKE_1(); }
    | ExpressionStatement { MAKE_1(); }
    | ReturnStatement { MAKE_1(); }
    ;

ExpressionStatement:
    StatementExpression SEMI_COLON { MAKE_2(symbol_kind::S_ExpressionStatement); }
    ;

StatementExpression:
    Assignment { MAKE_1(); }
    | MethodInvocation { MAKE_1(); }
    | ClassInstanceCreationExpression { MAKE_1(); }
    ;

StatementNoShortIf:
    StatementWithoutTrailingSubstatement { MAKE_1(); }
    | IfThenElseStatementNoShortIf { MAKE_1(); }
    | WhileStatementNoShortIf { MAKE_1(); }
    | ForStatementNoShortIf { MAKE_1(); }
    ;

EmptyStatement:
    SEMI_COLON { MAKE_1(); }
	;

IfThenStatement:
	IF ParExpression Statement { MAKE_3(symbol_kind::S_IfThenStatement); }
    ;

IfThenElseStatement:
	IF ParExpression StatementNoShortIf ELSE Statement { MAKE_5(symbol_kind::S_IfThenElseStatement); }
    ;

IfThenElseStatementNoShortIf:
	IF ParExpression StatementNoShortIf ELSE StatementNoShortIf { MAKE_5(symbol_kind::S_IfThenElseStatementNoShortIf); }
    ;

WhileStatement:
	  WHILE ParExpression Statement { MAKE_3(symbol_kind::S_WhileStatement); }
    ;

WhileStatementNoShortIf:
	  WHILE ParExpression StatementNoShortIf { MAKE_3(symbol_kind::S_WhileStatementNoShortIf); }
    ;

ForStatement:
	  FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN Statement
          { MAKE_NINE(symbol_kind::S_ForStatement); }
    ;

ForStatementNoShortIf:
	  FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN StatementNoShortIf
          { MAKE_NINE(symbol_kind::S_ForStatementNoShortIf); }
    ;
		
ForInitOpt:
    /* No init */ { MAKE_EMPTY(); }
    | ForInit { MAKE_1(); }
    ;

ForInit:
    StatementExpression { MAKE_1(); }
    | LocalVariableDeclaration { MAKE_1(); }
    ;

ForUpdateOpt:
    /* Empty - no update */ { MAKE_EMPTY(); }
    | StatementExpression { MAKE_1(); }
    ;

ExpressionOpt:
    /* no expression */ { MAKE_EMPTY(); }
    | Expression { MAKE_1(); }
    ;

ReturnStatement:
    RETURN ExpressionOpt SEMI_COLON { MAKE_3(symbol_kind::S_ReturnStatement); }
    ;

ParExpression:
    OPENING_PAREN Expression CLOSING_PAREN { MAKE_3(symbol_kind::S_ParExpression); }
    ;

QualifiedIdentifier:
    Identifier { MAKE_1(); }
    | QualifiedIdentifier DOT Identifier { MAKE_3(symbol_kind::S_QualifiedIdentifier); }
    ;

Identifier:
    IDENTIFIER { MAKE_1(); }
    ;

// Delay Type reduce due to conflict
LocalVariableDeclaration:
    // Type VariableDeclarators
    QualifiedIdentifier VariableDeclarator // ClassOrInterfaceType VariableDeclarators
        { MAKE_2(symbol_kind::S_LocalVariableDeclaration); }
    | QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET VariableDeclarator // ClassOrInterfaceTypeArray VariableDeclarators
        { MAKE_4(symbol_kind::S_LocalVariableDeclaration); }
    | PrimitiveType OPENING_BRACKET CLOSING_BRACKET VariableDeclarator // PrimitiveArray VariableDeclarators
        { MAKE_4(symbol_kind::S_LocalVariableDeclaration); }
    | PrimitiveType VariableDeclarator { MAKE_2(symbol_kind::S_LocalVariableDeclaration); }
    ;

Block:
    OPENING_BRACE BlockStatementsOpt CLOSING_BRACE { MAKE_3(symbol_kind::S_Block); }
    ;

BlockStatementsOpt:
    /* Empty - represents zero BlockStatements */ { MAKE_EMPTY(); }
    | BlockStatements { MAKE_1(); }
    ;

BlockStatements:
    BlockStatement { MAKE_1(); }
    | BlockStatements BlockStatement { MAKE_2(symbol_kind::S_BlockStatements); }
    ;

BlockStatement:
    LocalVariableDeclarationStatement  { MAKE_1(); }
    | ClassDeclaration { MAKE_1(); }
    | Statement { MAKE_1(); }
    ;

LocalVariableDeclarationStatement:
    LocalVariableDeclaration SEMI_COLON { MAKE_2(symbol_kind::S_LocalVariableDeclarationStatement); }
    ;

VariableDeclarator:
    VariableDeclaratorId { MAKE_1(); }
    | VariableDeclaratorId ASSIGNMENT VariableInitializer { MAKE_3(symbol_kind::S_VariableDeclarator); }
    ;

// -------------------------------------------------------------

%%

void yy::parser::error (const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << '\n';
}
