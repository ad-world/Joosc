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
  # include "../../ast/ast.h"
  # include "../../variant-ast/astnode.h"
  #define COMMA ,

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
%token <AstNodeVariant*> IF
%token <AstNodeVariant*> WHILE
%token <AstNodeVariant*> FOR
%token <AstNodeVariant*> ELSE
%token <AstNodeVariant*> EXTENDS
%token <AstNodeVariant*> IMPLEMENTS
%token <Modifier> PUBLIC
%token <Modifier> PROTECTED
%token <Modifier> STATIC
%token <Modifier> ABSTRACT
%token <Modifier> NATIVE
%token <Modifier> FINAL
%token <AstNodeVariant*> THIS
%token <AstNodeVariant*> VOID
%token <AstNodeVariant*> IMPORT
%token <AstNodeVariant*> CLASS
%token <AstNodeVariant*> NEW
%token <AstNodeVariant*> INSTANCEOF

// might need to look at this again
%token <AstNodeVariant*> PACKAGE
%token <AstNodeVariant*> INTERFACE
%token <AstNodeVariant*> OPENING_BRACE
%token <AstNodeVariant*> CLOSING_BRACE
%token <AstNodeVariant*> OPENING_PAREN
%token <AstNodeVariant*> CLOSING_PAREN
%token <AstNodeVariant*> OPENING_BRACKET
%token <AstNodeVariant*> CLOSING_BRACKET
%token <AstNodeVariant*> SEMI_COLON
%token <AstNodeVariant*> COLON
%token <AstNodeVariant*> COMMA
%token <AstNodeVariant*> DOT
%token <string>          IDENTIFIER
%token <AstNodeVariant*> ASSIGNMENT
%token <AstNodeVariant*> RETURN

// types
%token <AstNodeVariant*> INT
%token <AstNodeVariant*> BOOLEAN
%token <AstNodeVariant*> CHAR
%token <AstNodeVariant*> BYTE
%token <AstNodeVariant*> SHORT
%token <AstNodeVariant*> TRUE
%token <AstNodeVariant*> FALSE
%token <AstNodeVariant*> STRING_LITERAL
%token <AstNodeVariant*> INTEGER
%token <AstNodeVariant*> NULL_TOKEN
%token <AstNodeVariant*> CHAR_LITERAL

// comments
%token <AstNodeVariant*> MULTI_LINE_COMMENT
%token <AstNodeVariant*> SINGLE_LINE_COMMENT
%token <AstNodeVariant*> JAVADOC_COMMENT

// operators
%token <AstNodeVariant*> NEGATE
%token <AstNodeVariant*> ASTERISK
%token <AstNodeVariant*> PLUS
%token <AstNodeVariant*> MINUS
%token <AstNodeVariant*> DIVIDE
%token <AstNodeVariant*> MODULO
%token <AstNodeVariant*> LESS_THAN
%token <AstNodeVariant*> GREATER_THAN
%token <AstNodeVariant*> LESS_THAN_EQUAL
%token <AstNodeVariant*> GREATER_THAN_EQUAL
%token <AstNodeVariant*> BOOLEAN_EQUAL
%token <AstNodeVariant*> NOT_EQUAL
%token <AstNodeVariant*> AMPERSAND
%token <AstNodeVariant*> PIPE
%token <AstNodeVariant*> BOOLEAN_AND
%token <AstNodeVariant*> BOOLEAN_OR

// END OF FILE TOKEN
%token <AstNodeVariant*> EOF 0
/*****************************************************************************/

/************************* NONTERMINALS *************************/

// Comma macro used to avoid issue with # of args in other macros
%nterm <unique_ptr<CompilationUnit>> CompilationUnit
    %nterm <unique_ptr<QualifiedIdentifier>> PackageDeclaration
    %nterm <pair<vector<QualifiedIdentifier> COMMA vector<QualifiedIdentifier>>> ImportDeclarations
        %nterm <pair<unique_ptr<QualifiedIdentifier> COMMA unique_ptr<QualifiedIdentifier>>> ImportDeclaration
            %nterm <unique_ptr<QualifiedIdentifier>> SingleTypeImportDeclaration
            %nterm <unique_ptr<QualifiedIdentifier>> TypeImportOnDemandDeclaration
    %nterm <pair<vector<ClassDeclaration> COMMA vector<InterfaceDeclaration>>> TypeDeclarations
        %nterm <pair<unique_ptr<ClassDeclaration> COMMA unique_ptr<InterfaceDeclaration>>> TypeDeclaration
            %nterm <unique_ptr<ClassDeclaration>> ClassDeclaration
                // Modifiers
                // Identifier
                %nterm <unique_ptr<QualifiedIdentifier>> ExtendsOpt
                %nterm <vector<QualifiedIdentifier>> InterfacesOpt
                %nterm <pair<FieldDeclaration COMMA MethodDeclaration>> ClassBody
                    %nterm <unique_ptr<pair<vector<FieldDeclaration> COMMA vector<MethodDeclaration>>>> ClassBodyDeclarationsOpt
                    %nterm <pair<vector<FieldDeclaration> COMMA vector<MethodDeclaration>>> ClassBodyDeclarations
                        %nterm <pair<unique_ptr<FieldDeclaration> COMMA unique_ptr<MethodDeclaration>>> ClassBodyDeclaration
                            %nterm <pair<unique_ptr<FieldDeclaration> COMMA unique_ptr<MethodDeclaration>>> ClassMemberDeclaration
                                %nterm <unique_ptr<FieldDeclaration>> FieldDeclaration
                                    // Modifiers
                                    // Type
                                    // VariableDeclarator
                                %nterm <unique_ptr<MethodDeclaration>> MethodDeclaration
                                    // MethodHeader
                                    // MethodBody
            %nterm <unique_ptr<InterfaceDeclaration>> InterfaceDeclaration

%nterm <QualifiedIdentifier> QualifiedIdentifier
    %nterm <unique_ptr<Identifier>> Identifier

%nterm <vector<Modifier>> Modifiers
    %nterm <Modifier> Modifier

%nterm <AstNodeVariant*> Type

%nterm <AstNodeVariant*> Expression
%nterm <AstNodeVariant*> AssignmentExpression
%nterm <AstNodeVariant*> Assignment
%nterm <AstNodeVariant*> LeftHandSide
%nterm <AstNodeVariant*> ConditionalOrExpression
%nterm <AstNodeVariant*> ConditionalAndExpression
%nterm <AstNodeVariant*> InclusiveOrExpression
%nterm <AstNodeVariant*> AndExpression
%nterm <AstNodeVariant*> EqualityExpression
%nterm <AstNodeVariant*> RelationalExpression
%nterm <AstNodeVariant*> AdditiveExpression
%nterm <AstNodeVariant*> MultiplicativeExpression
%nterm <AstNodeVariant*> UnaryExpression
%nterm <AstNodeVariant*> UnaryExpressionNotPlusMinus
%nterm <AstNodeVariant*> CastExpression
%nterm <AstNodeVariant*> PrimaryOrExpressionName
%nterm <AstNodeVariant*> Primary
%nterm <AstNodeVariant*> ArrayCreationExpression
%nterm <AstNodeVariant*> ClassInstanceCreationExpression
%nterm <AstNodeVariant*> PrimaryNoNewArray
%nterm <AstNodeVariant*> Literal
%nterm <AstNodeVariant*> ArrayAccess
%nterm <AstNodeVariant*> FieldAccess
%nterm <AstNodeVariant*> ArgumentListOpt
%nterm <AstNodeVariant*> ArgumentList
%nterm <AstNodeVariant*> Arguments
%nterm <AstNodeVariant*> MethodInvocation
%nterm <AstNodeVariant*> PrimitiveType
%nterm <AstNodeVariant*> IntegralType
%nterm <AstNodeVariant*> BooleanType
%nterm <AstNodeVariant*> ClassOrInterfaceType
%nterm <AstNodeVariant*> ReferenceType
%nterm <AstNodeVariant*> InterfaceModifiersOpt
%nterm <AstNodeVariant*> InterfaceType
%nterm <AstNodeVariant*> ExtendsInterfaces
%nterm <AstNodeVariant*> ExtendsInterfacesOpt
%nterm <AstNodeVariant*> InterfaceBody
%nterm <AstNodeVariant*> InterfaceMemberDeclarationsOpt
%nterm <AstNodeVariant*> InterfaceMemberDeclarations
%nterm <AstNodeVariant*> InterfaceMemberDeclaration
%nterm <AstNodeVariant*> AbstractMethodDeclaration
%nterm <AstNodeVariant*> AbstractMethodModifiersOpt
%nterm <AstNodeVariant*> AbstractMethodModifiers
%nterm <AstNodeVariant*> Interfaces
%nterm <AstNodeVariant*> InterfaceTypeList
%nterm <AstNodeVariant*> VariableInitializer
%nterm <AstNodeVariant*> MethodHeader
%nterm <AstNodeVariant*> MethodDeclarator
%nterm <AstNodeVariant*> MethodBody
%nterm <AstNodeVariant*> FormalParameterListOpt
%nterm <AstNodeVariant*> FormalParameterList
%nterm <AstNodeVariant*> AbstractMethodModifier
%nterm <AstNodeVariant*> FormalParameter
%nterm <AstNodeVariant*> VariableDeclaratorId
%nterm <AstNodeVariant*> Statement
%nterm <AstNodeVariant*> StatementWithoutTrailingSubstatement
%nterm <AstNodeVariant*> ExpressionStatement
%nterm <AstNodeVariant*> StatementExpression
%nterm <AstNodeVariant*> StatementNoShortIf
%nterm <AstNodeVariant*> EmptyStatement
%nterm <AstNodeVariant*> IfThenStatement
%nterm <AstNodeVariant*> IfThenElseStatement
%nterm <AstNodeVariant*> IfThenElseStatementNoShortIf
%nterm <AstNodeVariant*> WhileStatement
%nterm <AstNodeVariant*> WhileStatementNoShortIf
%nterm <AstNodeVariant*> ForStatement
%nterm <AstNodeVariant*> ForStatementNoShortIf
%nterm <AstNodeVariant*> ForInitOpt
%nterm <AstNodeVariant*> ForInit
%nterm <AstNodeVariant*> ForUpdateOpt
%nterm <AstNodeVariant*> ExpressionOpt
%nterm <AstNodeVariant*> ReturnStatement
%nterm <AstNodeVariant*> ParExpression
%nterm <AstNodeVariant*> LocalVariableDeclaration
%nterm <AstNodeVariant*> Block
%nterm <AstNodeVariant*> BlockStatementsOpt
%nterm <AstNodeVariant*> BlockStatements
%nterm <AstNodeVariant*> BlockStatement
%nterm <AstNodeVariant*> LocalVariableDeclarationStatement
%nterm <AstNodeVariant*> VariableDeclarator
/******************** END NONTERMINALS ********************/

%parse-param {AstNodeVariant **root}

%{
#define MAKE_EMPTY(me)      ; // me = new AstNodeVariant((symbol_kind::S_YYEMPTY))
#define MAKE_ONE(me, you)   ; // me = you
#define MAKE_NODE(me, symbol, children...) ;
    // me = new AstNodeVariant((symbol)); me->addChild(children)

#define MAKE_STACK_OBJ(me, type, constructor...) \
    me = type(constructor)

#define NEW_OBJ(type, constructor...) \
    make_unique<type>(constructor)

#define MAKE_OBJ(me, type, constructor...) \
    me = make_unique<type>((constructor))

#define EMPTY \
    (nullptr)

#define EMPTY_MODIFIERS \
    (vector<Modifier>())

#define MAKE_CompilationUnit(me, package, importdecl_first, importdecl_second, typedecl_first, typedecl_second) \
    MAKE_OBJ(me, CompilationUnit, (package), (importdecl_first), \
        (importdecl_second), (typedecl_first), (typedecl_second))

// expects (me, pair<vector<type1>, vector<type2>>, pair<type1, type2>)
#define MAKE_PAIRVECTORS(me, pair_of_vector, pair) \
    if ( pair.first ) { pair_of_vector.first.push_back(*pair.first); } \
    if ( pair.second ) { pair_of_vector.second.push_back(*pair.second); } \
    me = pair_of_vector

// expects (me, vector, item)
#define MAKE_VECTOR(me, vector, item) \
    vector.push_back((item)); \
    me = vector
%}

// Grammar
%%
%start Start;

/*---------------------- Packages ----------------------*/

Start:
     CompilationUnit { /* *root = $1; */ }

CompilationUnit:
    PackageDeclaration ImportDeclarations TypeDeclarations
        { MAKE_CompilationUnit($$, $1,     $2.first, $2.second,     $3.first, $3.second); }
    | ImportDeclarations TypeDeclarations   // No PackageDeclaration
        { MAKE_CompilationUnit($$, EMPTY,     $1.first, $1.second,     $2.first, $2.second); }
    | PackageDeclaration TypeDeclarations   // No ImportDeclarations
        { MAKE_CompilationUnit($$, $1,     EMPTY, EMPTY,     $2.first, $2.second); }
    | PackageDeclaration ImportDeclarations // No TypeDeclarations
        { MAKE_CompilationUnit($$, $1,     $2.first, $2.second,     EMPTY, EMPTY); }
    | PackageDeclaration { MAKE_CompilationUnit($$, $1,     EMPTY, EMPTY,     EMPTY, EMPTY); }
    | ImportDeclarations { MAKE_CompilationUnit($$, EMPTY,    $1.first, $1.second,     EMPTY, EMPTY); }
    | TypeDeclarations { MAKE_CompilationUnit($$, EMPTY,     EMPTY, EMPTY,     $1.first, $1.second); }
    | /* Empty */ { MAKE_CompilationUnit($$, EMPTY,     EMPTY, EMPTY,     EMPTY, EMPTY); }
    ;

PackageDeclaration:
    PACKAGE QualifiedIdentifier SEMI_COLON
        { MAKE_OBJ($$, QualifiedIdentifier, $1); }
    ;

ImportDeclarations:
    ImportDeclaration
        { MAKE_STACK_OBJ($$, pair<vector<QualifiedIdentifier> COMMA vector<QualifiedIdentifier>>);
            MAKE_PAIRVECTORS($$, $$, $1); }
    | ImportDeclarations ImportDeclaration
        { MAKE_PAIRVECTORS($$, $1, $2); }
    ;

TypeDeclarations:
    TypeDeclaration
        { MAKE_STACK_OBJ($$, pair<vector<ClassDeclaration> COMMA vector<InterfaceDeclaration>>);
            MAKE_PAIRVECTORS($$, $$, $1); }
    | TypeDeclarations TypeDeclaration { MAKE_PAIRVECTORS($$, $1, $2); }
    ;

ImportDeclaration:
	SingleTypeImportDeclaration { $$ = make_pair($1, EMPTY); }
	| TypeImportOnDemandDeclaration { $$ = make_pair(EMPTY, $1); }
    ;

SingleTypeImportDeclaration:
    IMPORT QualifiedIdentifier SEMI_COLON // TypeName
        { MAKE_OBJ($$, QualifiedIdentifier, $2); }
    ;

TypeImportOnDemandDeclaration:
    IMPORT QualifiedIdentifier DOT ASTERISK SEMI_COLON // PackageOrTypeName
        { MAKE_OBJ($$, QualifiedIdentifier, $2); }
    ;

TypeDeclaration:
    ClassDeclaration { $$ = make_pair($1, EMPTY); }
    | InterfaceDeclaration { $$ = make_pair(EMPTY, $1); }
    | SEMI_COLON { $$ = make_pair(EMPTY, EMPTY); }
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
    Modifier { MAKE_STACK_OBJ($$, vector<Modifier>); MAKE_VECTOR($$, $$, $1); }
    | Modifiers Modifier { MAKE_VECTOR($$, $1, $2); }
    ;

Modifier:
    PUBLIC  { $$ = $1; }
    | PROTECTED  { $$ = $1; }
    | ABSTRACT  { $$ = $1; }
    | STATIC  { $$ = $1; }
    | NATIVE  { $$ = $1; }
    | FINAL { $$ = $1; }
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
        { MAKE_OBJ($$, ClassDeclaration, EMPTY_MODIFIERS, $1, $2, $3, $4); }
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
    /* Empty - No extends */ { $$ = EMPTY; }
    | EXTENDS QualifiedIdentifier { MAKE_OBJ($$, QualifiedIdentifier, $2); } // ClassType
    ;

/* Class body */
ClassBodyDeclarationsOpt:
    /* Empty */ { $$ = EMPTY; }
    | ClassBodyDeclarations { MAKE_OBJ($$, pair<vector<FieldDeclaration> COMMA vector<MethodDeclaration>>, $1); }
    ;

ClassBodyDeclarations:
    ClassBodyDeclaration {
            MAKE_STACK_OBJ($$, pair<vector<FieldDeclaration> COMMA vector<MethodDeclaration>>);
            MAKE_PAIRVECTORS($$, $$, $1);
        }
    | ClassBodyDeclarations ClassBodyDeclaration { MAKE_PAIRVECTORS($$, $1, $2); }
    ;

ClassBodyDeclaration:
    ClassMemberDeclaration { $$ = $1; }
    ;
    // ensure there is at least one constructor
    // | InstanceInitializer: omitted from joos
    // | StaticInitializer: omitted from joos

ClassMemberDeclaration:
    FieldDeclaration { $$ = {$1, EMPTY}; }
    | MethodDeclaration { $$ = {EMPTY, $1}; }
    | SEMI_COLON { $$ = {EMPTY, EMPTY}; }
    ;
    // | ClassDeclaration: omitted for joos
    // | InterfaceDeclaration: NOT SURE IF THIS SHOULD BE OMITTED FOR JOOS

ClassBody:
    OPENING_BRACE ClassBodyDeclarationsOpt CLOSING_BRACE { MAKE_NODE($$, symbol_kind::S_ClassBody, $1, $2, $3); }
    ;

/* Fields */
// Only one variable declaration is allowed at a time
FieldDeclaration:
    Type VariableDeclarator SEMI_COLON { MAKE_OBJ($$, FieldDeclaration, EMPTY_MODIFIERS, $1, $2); }
    | Modifiers Type VariableDeclarator SEMI_COLON { MAKE_OBJ($$, FieldDeclaration, $1, $2, $3); }
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

FormalParameter:
	Type VariableDeclaratorId { MAKE_NODE($$, symbol_kind::S_FormalParameter, $1, $2); }
    ;

VariableDeclaratorId:
    Identifier { MAKE_ONE($$, $1); }
    // | Identifier OPENING_BRACKET CLOSING_BRACKET { MAKE_NODE($$, symbol_kind::S_VariableDeclaratorId, $1, $2, $3); }
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
    Identifier { MAKE_STACK_OBJ($$, QualifiedIdentifier); MAKE_VECTOR($$, $$.identifiers, *$1); }
    | QualifiedIdentifier DOT Identifier { MAKE_VECTOR($$, $1.identifiers, *$3); }
    ;

Identifier:
    IDENTIFIER { MAKE_OBJ($$, Identifier, $1); }
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
