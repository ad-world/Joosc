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
%nterm <AstNode*> InclusiveOrExpression
%nterm <AstNode*> AndExpression
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
#define MAKE_EMPTY(me)      me = new AstNode((symbol_kind::S_YYEMPTY))
#define MAKE_ONE(me, you)   me = you
#define MAKE_NODE(me, symbol, children...) \
    me = new AstNode((symbol)); me->addChild(children)
%}

// Grammar
%%
%start Start;

/*---------------------- Packages ----------------------*/

Start:
     CompilationUnit { *root = $1; }

CompilationUnit:
    PackageDeclaration ImportDeclarations TypeDeclarations
        { MAKE_NODE($$, symbol_kind::S_CompilationUnit, $1, $2, $3); }
    | ImportDeclarations TypeDeclarations   // No PackageDeclaration
        { MAKE_NODE($$, symbol_kind::S_CompilationUnit, $1, $2); }
    | PackageDeclaration TypeDeclarations   // No ImportDeclarations
        { MAKE_NODE($$, symbol_kind::S_CompilationUnit, $1, $2); }
    | PackageDeclaration ImportDeclarations // No TypeDeclarations
        { MAKE_NODE($$, symbol_kind::S_CompilationUnit, $1, $2); }
    | PackageDeclaration { MAKE_ONE($$, $1); }
    | ImportDeclaration { MAKE_ONE($$, $1); }
    | TypeDeclaration { MAKE_ONE($$, $1); }
    | /* Empty */ { MAKE_EMPTY($$); }
    ;

PackageDeclaration:
    PACKAGE QualifiedIdentifier SEMI_COLON
        { MAKE_NODE($$, symbol_kind::S_PackageDeclaration, $1, $2, $3); }
    ;

ImportDeclarations:
    ImportDeclaration { MAKE_ONE($$, $1); }
    | ImportDeclarations ImportDeclaration
        { MAKE_NODE($$, symbol_kind::S_ImportDeclarations, $1, $2); }
    ;

TypeDeclarations:
    TypeDeclaration { MAKE_ONE($$, $1); }
    | TypeDeclarations TypeDeclaration { MAKE_NODE($$, symbol_kind::S_TypeDeclarations, $1, $2); }
    ;

ImportDeclaration:
	SingleTypeImportDeclaration { MAKE_ONE($$, $1); }
	| TypeImportOnDemandDeclaration { MAKE_ONE($$, $1); }
    ;

SingleTypeImportDeclaration:
    IMPORT QualifiedIdentifier SEMI_COLON // TypeName
        { MAKE_NODE($$, symbol_kind::S_SingleTypeImportDeclaration, $1, $2, $3); }
    ;

TypeImportOnDemandDeclaration:
    IMPORT QualifiedIdentifier DOT ASTERISK SEMI_COLON // PackageOrTypeName
        { MAKE_NODE($$, symbol_kind::S_TypeImportOnDemandDeclaration, $1, $2, $3, $4, $5); }
    ;

TypeDeclaration:
    ClassDeclaration { MAKE_ONE($$, $1); }
    | InterfaceDeclaration { MAKE_ONE($$, $1); }
    | SEMI_COLON { MAKE_ONE($$, $1); }
    ;

/*---------------------- Expressions ----------------------*/

Expression:
    AssignmentExpression { MAKE_ONE($$, $1); }
    ;

AssignmentExpression:
    Assignment { MAKE_ONE($$, $1); }
    | ConditionalOrExpression { MAKE_ONE($$, $1); }
    ;

Assignment:
    LeftHandSide ASSIGNMENT AssignmentExpression { MAKE_NODE($$, symbol_kind::S_Assignment, $1, $2, $3); }

LeftHandSide:
    QualifiedIdentifier // ExpressionName
        { MAKE_ONE($$, $1); }
    | FieldAccess { MAKE_ONE($$, $1); }
    | ArrayAccess { MAKE_ONE($$, $1); }
    ;

ConditionalOrExpression:
    ConditionalAndExpression { MAKE_ONE($$, $1); }
    | ConditionalOrExpression BOOLEAN_OR ConditionalAndExpression
        { MAKE_NODE($$, symbol_kind::S_ConditionalOrExpression, $1, $2, $3); }
    ;

ConditionalAndExpression:
    InclusiveOrExpression { MAKE_ONE($$, $1); }
    | ConditionalAndExpression BOOLEAN_AND InclusiveOrExpression
        { MAKE_NODE($$, symbol_kind::S_ConditionalAndExpression, $1, $2, $3); }
    ;

InclusiveOrExpression:
    AndExpression { MAKE_ONE($$, $1); }
    | InclusiveOrExpression PIPE AndExpression { MAKE_NODE($$, symbol_kind::S_InclusiveOrExpression, $1, $2, $3); }

AndExpression:
    EqualityExpression { MAKE_ONE($$, $1); }
    | AndExpression AMPERSAND EqualityExpression { MAKE_NODE($$, symbol_kind::S_AndExpression, $1, $2, $3); }

EqualityExpression:
    RelationalExpression { MAKE_ONE($$, $1); }
    | EqualityExpression BOOLEAN_EQUAL RelationalExpression  { MAKE_NODE($$, symbol_kind::S_EqualityExpression, $1, $2, $3); }
    | EqualityExpression NOT_EQUAL RelationalExpression { MAKE_NODE($$, symbol_kind::S_EqualityExpression, $1, $2, $3); }
    ;

RelationalExpression:
    AdditiveExpression { MAKE_ONE($$, $1); }
    | RelationalExpression LESS_THAN AdditiveExpression
        { MAKE_NODE($$, symbol_kind::S_RelationalExpression, $1, $2, $3); }
    | RelationalExpression GREATER_THAN AdditiveExpression
        { MAKE_NODE($$, symbol_kind::S_RelationalExpression, $1, $2, $3); }
    | RelationalExpression LESS_THAN_EQUAL AdditiveExpression
        { MAKE_NODE($$, symbol_kind::S_RelationalExpression, $1, $2, $3); }
    | RelationalExpression GREATER_THAN_EQUAL AdditiveExpression
        { MAKE_NODE($$, symbol_kind::S_RelationalExpression, $1, $2, $3); }
    | RelationalExpression INSTANCEOF ReferenceType // ReferenceType
        { MAKE_NODE($$, symbol_kind::S_RelationalExpression, $1, $2, $3); }
    ;

AdditiveExpression:
    MultiplicativeExpression { MAKE_ONE($$, $1); }
    | AdditiveExpression PLUS MultiplicativeExpression { MAKE_NODE($$, symbol_kind::S_AdditiveExpression, $1, $2, $3); }
    | AdditiveExpression MINUS MultiplicativeExpression { MAKE_NODE($$, symbol_kind::S_AdditiveExpression, $1, $2, $3); }
    ;

MultiplicativeExpression:
    UnaryExpression { MAKE_ONE($$, $1); }
    | MultiplicativeExpression ASTERISK UnaryExpression { MAKE_NODE($$, symbol_kind::S_MultiplicativeExpression, $1, $2, $3); }
    | MultiplicativeExpression DIVIDE UnaryExpression { MAKE_NODE($$, symbol_kind::S_MultiplicativeExpression, $1, $2, $3); }
    | MultiplicativeExpression MODULO UnaryExpression { MAKE_NODE($$, symbol_kind::S_MultiplicativeExpression, $1, $2, $3); }
    ;

UnaryExpression:
    MINUS UnaryExpression { MAKE_NODE($$, symbol_kind::S_UnaryExpression, $1, $2); }
    | UnaryExpressionNotPlusMinus { MAKE_ONE($$, $1); }
    ;

UnaryExpressionNotPlusMinus:
    NEGATE UnaryExpression { MAKE_NODE($$, symbol_kind::S_UnaryExpressionNotPlusMinus, $1, $2); }
    | CastExpression { MAKE_ONE($$, $1); }
    | PrimaryOrExpressionName { MAKE_ONE($$, $1); }
    ;

CastExpression: // Done this way to avoid conflicts
    OPENING_PAREN PrimitiveType CLOSING_PAREN UnaryExpression
        { MAKE_NODE($$, symbol_kind::S_CastExpression, $1, $2, $3, $4); }
    | OPENING_PAREN Expression CLOSING_PAREN UnaryExpressionNotPlusMinus // Expression must be verified to be QualifiedIdentifier (ReferenceType no array)
        { MAKE_NODE($$, symbol_kind::S_CastExpression, $1, $2, $3, $4); }
    | OPENING_PAREN QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET CLOSING_PAREN UnaryExpressionNotPlusMinus // ReferenceType with array
        { MAKE_NODE($$, symbol_kind::S_CastExpression, $1, $2, $3, $4, $5, $6); }
    | OPENING_PAREN
        PrimitiveType OPENING_BRACKET CLOSING_BRACKET // ReferenceType as PrimitiveType with array
            CLOSING_PAREN UnaryExpressionNotPlusMinus
        { MAKE_NODE($$, symbol_kind::S_CastExpression, $1, $2, $3, $4, $5, $6); }
    ;

PrimaryOrExpressionName:
    Primary { MAKE_ONE($$, $1); }
    | QualifiedIdentifier // ExpressionName
        { MAKE_ONE($$, $1); }
    ;

Primary:
    PrimaryNoNewArray { MAKE_ONE($$, $1); }
    | ArrayCreationExpression { MAKE_ONE($$, $1); }
    ;

ArrayCreationExpression:
    NEW PrimitiveType OPENING_BRACKET CLOSING_BRACKET
        { MAKE_NODE($$, symbol_kind::S_ArrayCreationExpression, $1, $2, $3, $4); }
    | NEW PrimitiveType OPENING_BRACKET Expression CLOSING_BRACKET
        { MAKE_NODE($$, symbol_kind::S_ArrayCreationExpression, $1, $2, $3, $4, $5); }
    | NEW QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET // TypeName
        { MAKE_NODE($$, symbol_kind::S_ArrayCreationExpression, $1, $2, $3, $4); }
    | NEW QualifiedIdentifier OPENING_BRACKET Expression CLOSING_BRACKET // TypeName
        { MAKE_NODE($$, symbol_kind::S_ArrayCreationExpression, $1, $2, $3, $4, $5); }
    ;

ClassInstanceCreationExpression:
    NEW QualifiedIdentifier OPENING_PAREN ArgumentListOpt CLOSING_PAREN
        { MAKE_NODE($$, symbol_kind::S_ClassInstanceCreationExpression, $1, $2, $3, $4, $5); }

PrimaryNoNewArray:
    Literal { MAKE_ONE($$, $1); }
    | THIS { MAKE_ONE($$, $1); }
    | QualifiedIdentifier DOT THIS // ClassName
        { MAKE_NODE($$, symbol_kind::S_PrimaryNoNewArray, $1, $2, $3); }
    | OPENING_PAREN Expression CLOSING_PAREN
        { MAKE_NODE($$, symbol_kind::S_PrimaryNoNewArray, $1, $2, $3); }
    | ClassInstanceCreationExpression { MAKE_ONE($$, $1); }
    | FieldAccess { MAKE_ONE($$, $1); }
    | MethodInvocation { MAKE_ONE($$, $1); }
    | ArrayAccess { MAKE_ONE($$, $1); }
    ;

Literal:
    INTEGER  { MAKE_ONE($$, $1); }
    | TRUE  { MAKE_ONE($$, $1); }
    | FALSE  { MAKE_ONE($$, $1); }
    | CHAR_LITERAL  { MAKE_ONE($$, $1); }
    | STRING_LITERAL  { MAKE_ONE($$, $1); }
    | NULL_TOKEN   { MAKE_ONE($$, $1); }
    ;

ArrayAccess:
    QualifiedIdentifier OPENING_BRACKET Expression CLOSING_BRACKET // ExpressionName
        { MAKE_NODE($$, symbol_kind::S_ArrayAccess, $1, $2, $3, $4); }
    | PrimaryNoNewArray OPENING_BRACKET Expression CLOSING_BRACKET
        { MAKE_NODE($$, symbol_kind::S_ArrayAccess, $1, $2, $3, $4); }
    ;

FieldAccess:
    Primary DOT Identifier
        { MAKE_NODE($$, symbol_kind::S_FieldAccess, $1, $2, $3); }
    ;

ArgumentListOpt:
    /* Empty - No arguments */ { MAKE_EMPTY($$); }
    | ArgumentList { MAKE_ONE($$, $1); }
    ;

ArgumentList:
    Expression { MAKE_ONE($$, $1); }
    | Expression COMMA ArgumentList
        { MAKE_NODE($$, symbol_kind::S_ArgumentList, $1, $2, $3); }
    ;

Arguments:
    OPENING_PAREN ArgumentListOpt CLOSING_PAREN
        { MAKE_NODE($$, symbol_kind::S_Arguments, $1, $2, $3); }
    ;

MethodInvocation:
    QualifiedIdentifier Arguments // MethodName
        { MAKE_NODE($$, symbol_kind::S_MethodInvocation, $1, $2); }
    | Primary DOT Identifier Arguments
        { MAKE_NODE($$, symbol_kind::S_MethodInvocation, $1, $2, $3, $4); }
    ;

Type:
    PrimitiveType { MAKE_ONE($$, $1); }
    | ReferenceType { MAKE_ONE($$, $1); }
    ;

PrimitiveType:
    IntegralType { MAKE_ONE($$, $1); }
    | BooleanType { MAKE_ONE($$, $1); }
    ;

IntegralType:
    BYTE { MAKE_ONE($$, $1); }
    | SHORT { MAKE_ONE($$, $1); }
    | INT { MAKE_ONE($$, $1); }
    | CHAR { MAKE_ONE($$, $1); }
    ;

BooleanType:
    BOOLEAN { MAKE_ONE($$, $1); }
    ;

ClassOrInterfaceType:
    QualifiedIdentifier // ClassType, InterfaceType -> TypeName
        { MAKE_ONE($$, $1); }

ReferenceType: // Done this way to disallow multidimensional arrays
    ClassOrInterfaceType
        { MAKE_ONE($$, $1); }
    | ClassOrInterfaceType OPENING_BRACKET CLOSING_BRACKET
        { MAKE_NODE($$, symbol_kind::S_ReferenceType, $1, $2, $3); }
    | PrimitiveType OPENING_BRACKET CLOSING_BRACKET
        { MAKE_NODE($$, symbol_kind::S_ReferenceType, $1, $2, $3); }
    ;

/*---------------------- Interfaces ----------------------*/

InterfaceDeclaration:
    InterfaceModifiersOpt INTERFACE Identifier ExtendsInterfacesOpt InterfaceBody
        { MAKE_NODE($$, symbol_kind::S_InterfaceDeclaration, $1, $2, $3, $4, $5); }
    ;

Modifiers:
    Modifier { MAKE_ONE($$, $1); }
    | Modifiers Modifier { MAKE_NODE($$, symbol_kind::S_Modifiers, $1, $2); }
    ;

Modifier:
    PUBLIC  { MAKE_ONE($$, $1); }
    | PROTECTED  { MAKE_ONE($$, $1); }
    | PRIVATE  { MAKE_ONE($$, $1); }
    | ABSTRACT  { MAKE_ONE($$, $1); }
    | STATIC  { MAKE_ONE($$, $1); }
    | NATIVE  { MAKE_ONE($$, $1); }
    | FINAL { MAKE_ONE($$, $1); }
    ;

InterfaceModifiersOpt:
    /* Empty - optional */ { MAKE_EMPTY($$); }
    | Modifiers { MAKE_ONE($$, $1); }
    ;

InterfaceType:
    QualifiedIdentifier { MAKE_ONE($$, $1); }
    ;

ExtendsInterfaces:
    EXTENDS InterfaceType { MAKE_NODE($$, symbol_kind::S_ExtendsInterfaces, $1, $2); }
    | ExtendsInterfaces COMMA InterfaceType { MAKE_NODE($$, symbol_kind::S_ExtendsInterfaces, $1, $2, $3); }
    ;

ExtendsInterfacesOpt:
    /* Empty - optional interface */ { MAKE_EMPTY($$); }
    | ExtendsInterfaces { MAKE_ONE($$, $1); }
    ;

InterfaceBody:
    OPENING_BRACE InterfaceMemberDeclarationsOpt CLOSING_BRACE
        { MAKE_NODE($$, symbol_kind::S_InterfaceBody, $1, $2, $3); }
    ;

InterfaceMemberDeclarationsOpt:
    /* Empty - No interface body declarations */ { MAKE_EMPTY($$); }
    | InterfaceMemberDeclarations { MAKE_ONE($$, $1); }
    ;

InterfaceMemberDeclarations:
    InterfaceMemberDeclaration { MAKE_ONE($$, $1); }
    | InterfaceMemberDeclarations InterfaceMemberDeclaration
        { MAKE_NODE($$, symbol_kind::S_InterfaceMemberDeclarations, $1, $2); }
    ;

InterfaceMemberDeclaration: // Nested types and interface constants not supported
    AbstractMethodDeclaration { MAKE_ONE($$, $1); }
    ;

AbstractMethodDeclaration:
    AbstractMethodModifiersOpt Type MethodDeclarator SEMI_COLON
        { MAKE_NODE($$, symbol_kind::S_AbstractMethodDeclaration, $1, $2, $3, $4); }
    | AbstractMethodModifiersOpt VOID MethodDeclarator SEMI_COLON
        { MAKE_NODE($$, symbol_kind::S_AbstractMethodDeclaration, $1, $2, $3, $4); }
    ;

AbstractMethodModifiersOpt:
    /* Empty - optional */ { MAKE_EMPTY($$); }
    | AbstractMethodModifiers { MAKE_ONE($$, $1); }
    ;

AbstractMethodModifiers:
    AbstractMethodModifier
        { MAKE_ONE($$, $1); }
    | AbstractMethodModifiers AbstractMethodModifier
        { MAKE_NODE($$, symbol_kind::S_AbstractMethodModifiers, $1, $2); }
    ;


/*---------------------- Classes ---------------------- JAGVIR START HERE */

// weeder: make sure at most one of implements and extends is present
// weeder: modifier can only be abstract, final, public
// weeder: max one of abstract/final
// weeder: must contain public?
ClassDeclaration:
    CLASS Identifier ExtendsOpt InterfacesOpt ClassBody
        { MAKE_NODE($$, symbol_kind::S_ClassDeclaration, $1, $2, $3, $4, $5); }
    | Modifiers CLASS Identifier ExtendsOpt InterfacesOpt ClassBody
        { MAKE_NODE($$, symbol_kind::S_ClassDeclaration, $1, $2, $3, $4, $5, $6); }
    ;

/* Class interfaces */
InterfacesOpt:
    /* Empty - No implements */ { MAKE_EMPTY($$); }
    | Interfaces { MAKE_ONE($$, $1); }
    ;

Interfaces:
    IMPLEMENTS InterfaceTypeList { MAKE_NODE($$, symbol_kind::S_Interfaces, $1, $2); }
    ;

InterfaceTypeList:
    InterfaceType { MAKE_ONE($$, $1); }
    | InterfaceTypeList COMMA InterfaceType { MAKE_NODE($$, symbol_kind::S_InterfaceTypeList, $1, $2, $3); }
    ;

/* Class Extends */
ExtendsOpt:
    /* Empty - No extends */ { MAKE_EMPTY($$); }
    | EXTENDS QualifiedIdentifier { MAKE_NODE($$, symbol_kind::S_ExtendsOpt, $1, $2); } // ClassType
    ;

/* Class body */
ClassBodyDeclarationsOpt:
    /* Empty */ { MAKE_EMPTY($$); }
    | ClassBodyDeclarations { MAKE_ONE($$, $1); }
    ;

ClassBodyDeclarations:
    ClassBodyDeclaration { MAKE_ONE($$, $1); }
    | ClassBodyDeclarations ClassBodyDeclaration { MAKE_NODE($$, symbol_kind::S_ClassBodyDeclarations, $1, $2); }
    ;

ClassBodyDeclaration:
    ClassMemberDeclaration { MAKE_ONE($$, $1); }
    ;
    // ensure there is at least one constructor
    // | InstanceInitializer: omitted from joos
    // | StaticInitializer: omitted from joos

ClassMemberDeclaration:
    FieldDeclaration { MAKE_ONE($$, $1); }
    | MethodDeclaration { MAKE_ONE($$, $1); }
    | SEMI_COLON { MAKE_ONE($$, $1); }
    ;
    // | ClassDeclaration: omitted for joos
    // | InterfaceDeclaration: NOT SURE IF THIS SHOULD BE OMITTED FOR JOOS

ClassBody:
    OPENING_BRACE ClassBodyDeclarationsOpt CLOSING_BRACE { MAKE_NODE($$, symbol_kind::S_ClassBody, $1, $2, $3); }
    ;

/* Fields */
// Only one variable declaration is allowed at a time
FieldDeclaration:
    Type VariableDeclarator SEMI_COLON { MAKE_NODE($$, symbol_kind::S_FieldDeclaration, $1, $2, $3); }
    | Modifiers Type VariableDeclarator SEMI_COLON { MAKE_NODE($$, symbol_kind::S_FieldDeclaration, $1, $2, $3, $4); }
    ;

VariableInitializer:
    Expression { MAKE_ONE($$, $1); }
    ;

/* Methods */
// weeder: methodbody exists if neither abstract nor native
MethodDeclaration: // One of these must be constructor
    MethodHeader MethodBody { MAKE_NODE($$, symbol_kind::S_MethodDeclaration, $1, $2); }
    ;

// weeder: allow static native int m(int)
// weeder: see A1 specs for weeding modifiers
MethodHeader:
    Type MethodDeclarator { MAKE_NODE($$, symbol_kind::S_MethodHeader, $1, $2); }
    | Modifiers Type MethodDeclarator { MAKE_NODE($$, symbol_kind::S_MethodHeader, $1, $2, $3); }
    | VOID MethodDeclarator { MAKE_NODE($$, symbol_kind::S_MethodHeader, $1, $2); }
    | Modifiers VOID MethodDeclarator { MAKE_NODE($$, symbol_kind::S_MethodHeader, $1, $2, $3); }
    | Modifiers MethodDeclarator // Represents constructor, todo weeding: reject if identifier is not class name
        { MAKE_NODE($$, symbol_kind::S_MethodHeader, $1, $2); }
    ;

MethodDeclarator:
    Identifier OPENING_PAREN FormalParameterListOpt CLOSING_PAREN
        { MAKE_NODE($$, symbol_kind::S_MethodDeclarator, $1, $2, $3, $4); }
    ;

MethodBody:
    SEMI_COLON { MAKE_ONE($$, $1); }
    | Block { MAKE_ONE($$, $1); }
    ;

/* Formal parameters */
FormalParameterListOpt:
    { MAKE_EMPTY($$); }
    | FormalParameterList { MAKE_ONE($$, $1); }
    ;

FormalParameterList:
    FormalParameter { MAKE_ONE($$, $1); }
    | FormalParameterList COMMA FormalParameter { MAKE_NODE($$, symbol_kind::S_FormalParameterList, $1, $2, $3); }
    ;
/*-----------------------*/

AbstractMethodModifier:
    PUBLIC { MAKE_ONE($$, $1); }
    | ABSTRACT { MAKE_ONE($$, $1); }
    ;

FinalOpt:
    /* Empty - No final keyword */ { MAKE_EMPTY($$); }
    | FINAL { MAKE_ONE($$, $1); }
    ;

FormalParameter:
	FinalOpt Type VariableDeclaratorId { MAKE_NODE($$, symbol_kind::S_FormalParameter, $1, $2, $3); }
    ;

VariableDeclaratorId:
    Identifier { MAKE_ONE($$, $1); }
    | Identifier OPENING_BRACKET CLOSING_BRACKET { MAKE_NODE($$, symbol_kind::S_VariableDeclaratorId, $1, $2, $3); }
    ;

/*---------------------- Statements ----------------------*/

Statement:
    StatementWithoutTrailingSubstatement { MAKE_ONE($$, $1); }
    | IfThenStatement { MAKE_ONE($$, $1); }
    | IfThenElseStatement { MAKE_ONE($$, $1); }
    | WhileStatement { MAKE_ONE($$, $1); }
    | ForStatement { MAKE_ONE($$, $1); }
    ;

StatementWithoutTrailingSubstatement:
    Block { MAKE_ONE($$, $1); }
    | EmptyStatement { MAKE_ONE($$, $1); }
    | ExpressionStatement { MAKE_ONE($$, $1); }
    | ReturnStatement { MAKE_ONE($$, $1); }
    ;

ExpressionStatement:
    StatementExpression SEMI_COLON { MAKE_NODE($$, symbol_kind::S_ExpressionStatement, $1, $2); }
    ;

StatementExpression:
    Assignment { MAKE_ONE($$, $1); }
    | MethodInvocation { MAKE_ONE($$, $1); }
    | ClassInstanceCreationExpression { MAKE_ONE($$, $1); }
    ;

StatementNoShortIf:
    StatementWithoutTrailingSubstatement { MAKE_ONE($$, $1); }
    | IfThenElseStatementNoShortIf { MAKE_ONE($$, $1); }
    | WhileStatementNoShortIf { MAKE_ONE($$, $1); }
    | ForStatementNoShortIf { MAKE_ONE($$, $1); }
    ;

EmptyStatement:
    SEMI_COLON { MAKE_ONE($$, $1); }
	;

IfThenStatement:
	IF ParExpression Statement { MAKE_NODE($$, symbol_kind::S_IfThenStatement, $1, $2, $3); }
    ;

IfThenElseStatement:
	IF ParExpression StatementNoShortIf ELSE Statement { MAKE_NODE($$, symbol_kind::S_IfThenElseStatement, $1, $2, $3, $4, $5); }
    ;

IfThenElseStatementNoShortIf:
	IF ParExpression StatementNoShortIf ELSE StatementNoShortIf { MAKE_NODE($$, symbol_kind::S_IfThenElseStatementNoShortIf, $1, $2, $3, $4, $5); }
    ;

WhileStatement:
	  WHILE ParExpression Statement { MAKE_NODE($$, symbol_kind::S_WhileStatement, $1, $2, $3); }
    ;

WhileStatementNoShortIf:
	  WHILE ParExpression StatementNoShortIf { MAKE_NODE($$, symbol_kind::S_WhileStatementNoShortIf, $1, $2, $3); }
    ;

ForStatement:
	  FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN Statement
          { MAKE_NODE($$, symbol_kind::S_ForStatement, $1, $2, $3, $4, $5, $6, $7, $8, $9); }
    ;

ForStatementNoShortIf:
	  FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN StatementNoShortIf
          { MAKE_NODE($$, symbol_kind::S_ForStatementNoShortIf, $1, $2, $3, $4, $5, $6, $7, $8, $9); }
    ;
		
ForInitOpt:
    /* No init */ { MAKE_EMPTY($$); }
    | ForInit { MAKE_ONE($$, $1); }
    ;

ForInit:
    StatementExpression { MAKE_ONE($$, $1); }
    | LocalVariableDeclaration { MAKE_ONE($$, $1); }
    ;

ForUpdateOpt:
    /* Empty - no update */ { MAKE_EMPTY($$); }
    | StatementExpression { MAKE_ONE($$, $1); }
    ;

ExpressionOpt:
    /* no expression */ { MAKE_EMPTY($$); }
    | Expression { MAKE_ONE($$, $1); }
    ;

ReturnStatement:
    RETURN ExpressionOpt SEMI_COLON { MAKE_NODE($$, symbol_kind::S_ReturnStatement, $1, $2, $3); }
    ;

ParExpression:
    OPENING_PAREN Expression CLOSING_PAREN { MAKE_NODE($$, symbol_kind::S_ParExpression, $1, $2, $3); }
    ;

QualifiedIdentifier:
    Identifier { MAKE_ONE($$, $1); }
    | QualifiedIdentifier DOT Identifier { MAKE_NODE($$, symbol_kind::S_QualifiedIdentifier, $1, $2, $3); }
    ;

Identifier:
    IDENTIFIER { MAKE_ONE($$, $1); }
    ;

// Delay Type reduce due to conflict
LocalVariableDeclaration:
    // Type VariableDeclarators
    QualifiedIdentifier VariableDeclarator // ClassOrInterfaceType VariableDeclarators
        { MAKE_NODE($$, symbol_kind::S_LocalVariableDeclaration, $1, $2); }
    | QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET VariableDeclarator // ClassOrInterfaceTypeArray VariableDeclarators
        { MAKE_NODE($$, symbol_kind::S_LocalVariableDeclaration, $1, $2, $3, $4); }
    | PrimitiveType OPENING_BRACKET CLOSING_BRACKET VariableDeclarator // PrimitiveArray VariableDeclarators
        { MAKE_NODE($$, symbol_kind::S_LocalVariableDeclaration, $1, $2, $3, $4); }
    | PrimitiveType VariableDeclarator { MAKE_NODE($$, symbol_kind::S_LocalVariableDeclaration, $1, $2); }
    ;

Block:
    OPENING_BRACE BlockStatementsOpt CLOSING_BRACE { MAKE_NODE($$, symbol_kind::S_Block, $1, $2, $3); }
    ;

BlockStatementsOpt:
    /* Empty - represents zero BlockStatements */ { MAKE_EMPTY($$); }
    | BlockStatements { MAKE_ONE($$, $1); }
    ;

BlockStatements:
    BlockStatement { MAKE_ONE($$, $1); }
    | BlockStatements BlockStatement { MAKE_NODE($$, symbol_kind::S_BlockStatements, $1, $2); }
    ;

BlockStatement:
    LocalVariableDeclarationStatement  { MAKE_ONE($$, $1); }
    | ClassDeclaration { MAKE_ONE($$, $1); }
    | Statement { MAKE_ONE($$, $1); }
    ;

LocalVariableDeclarationStatement:
    LocalVariableDeclaration SEMI_COLON { MAKE_NODE($$, symbol_kind::S_LocalVariableDeclarationStatement, $1, $2); }
    ;

VariableDeclarator:
    VariableDeclaratorId { MAKE_ONE($$, $1); }
    | VariableDeclaratorId ASSIGNMENT VariableInitializer { MAKE_NODE($$, symbol_kind::S_VariableDeclarator, $1, $2, $3); }
    ;

// -------------------------------------------------------------

%%

void yy::parser::error (const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << '\n';
}
