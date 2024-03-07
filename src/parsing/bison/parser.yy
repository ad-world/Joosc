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
  # include "../../variant-ast/astnode.h"
  # include "../../add-location/add-location.h"
  #define COMMA ,

  class Driver;
  using namespace std;
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
%token <Modifier> PUBLIC
%token <Modifier> PROTECTED
%token <Modifier> STATIC
%token <Modifier> ABSTRACT
%token <Modifier> NATIVE
%token <Modifier> FINAL
%token THIS
%token IMPORT
%token CLASS
%token NEW

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
%token <string>          IDENTIFIER
%token ASSIGNMENT
%token RETURN

// types
%token <PrimitiveType> INT
%token <PrimitiveType> BOOLEAN
%token <PrimitiveType> CHAR
%token <PrimitiveType> BYTE
%token <PrimitiveType> SHORT
%token <PrimitiveType> VOID
%token <bool> TRUE
%token <bool> FALSE
%token <string> STRING_LITERAL
%token <int64_t> INTEGER
%token <nullptr_t> NULL_TOKEN
%token <string> CHAR_LITERAL

// comments
%token MULTI_LINE_COMMENT
%token SINGLE_LINE_COMMENT
%token JAVADOC_COMMENT

// operators
%token <InfixOperator> BOOLEAN_OR
%token <InfixOperator> BOOLEAN_AND
%token <InfixOperator> PIPE             // eager or
%token <InfixOperator> AMPERSAND        // eager and
%token <InfixOperator> BOOLEAN_EQUAL
%token <InfixOperator> NOT_EQUAL
%token <InfixOperator> PLUS
%token <InfixOperator> MINUS            // can also be used as a prefix operator
%token <InfixOperator> DIVIDE
%token <InfixOperator> ASTERISK         // multiply
%token <InfixOperator> LESS_THAN
%token <InfixOperator> GREATER_THAN
%token <InfixOperator> LESS_THAN_EQUAL
%token <InfixOperator> GREATER_THAN_EQUAL
%token INSTANCEOF
%token <InfixOperator> MODULO
%token <PrefixOperator> NEGATE

// END OF FILE TOKEN
%token EOF 0
/*****************************************************************************/

/************************* NONTERMINALS *************************/

// Comma macro used to avoid issue with # of args in other macros

// Overall tree (in progress)
%nterm <unique_ptr<CompilationUnit>> CompilationUnit
    %nterm <unique_ptr<QualifiedIdentifier>> PackageDeclaration
    %nterm <pair<vector<QualifiedIdentifier> COMMA vector<QualifiedIdentifier>>> ImportDeclarations
        %nterm <pair<unique_ptr<QualifiedIdentifier> COMMA unique_ptr<QualifiedIdentifier>>> ImportDeclaration
            %nterm <unique_ptr<QualifiedIdentifier>> SingleTypeImportDeclaration
            %nterm <unique_ptr<QualifiedIdentifier>> TypeImportOnDemandDeclaration
    %nterm <pair<vector<ClassDeclaration> COMMA vector<InterfaceDeclaration>>> TypeDeclarations
        %nterm <pair<unique_ptr<ClassDeclaration> COMMA unique_ptr<InterfaceDeclaration>>> TypeDeclaration
            // Class declaration
            // InterfaceDeclaration

// Class declaration (done?)
%nterm <unique_ptr<ClassDeclaration>> ClassDeclaration
    // Modifiers
    // Identifier
    %nterm <unique_ptr<QualifiedIdentifier>> ExtendsOpt
    %nterm <vector<QualifiedIdentifier>> InterfacesOpt
        %nterm <vector<QualifiedIdentifier>> Interfaces
            %nterm <vector<QualifiedIdentifier>> InterfaceTypeList
    %nterm <pair<vector<FieldDeclaration> COMMA vector<MethodDeclaration>>> ClassBody
        %nterm <pair<vector<FieldDeclaration> COMMA vector<MethodDeclaration>>> ClassBodyDeclarationsOpt
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

// Method Header/Body (done)
%nterm <unique_ptr<MethodDeclaration>> MethodHeader
    %nterm <pair<unique_ptr<Identifier> COMMA vector<FormalParameter>>> MethodDeclarator
        %nterm <vector<FormalParameter>> FormalParameterListOpt
            %nterm <vector<FormalParameter>> FormalParameterList
                %nterm <unique_ptr<FormalParameter>> FormalParameter
%nterm <unique_ptr<Block>> MethodBody
    // (Block)

// InterfaceDeclaration (done)
%nterm <unique_ptr<InterfaceDeclaration>> InterfaceDeclaration
    %nterm <vector<Modifier>> InterfaceModifiersOpt
    // Identifier
    %nterm <vector<QualifiedIdentifier>> ExtendsInterfacesOpt
        %nterm <vector<QualifiedIdentifier>> ExtendsInterfaces
            %nterm <unique_ptr<QualifiedIdentifier>> InterfaceType
    %nterm <vector<MethodDeclaration>> InterfaceBody
        %nterm <vector<MethodDeclaration>> InterfaceMemberDeclarationsOpt
            %nterm <vector<MethodDeclaration>> InterfaceMemberDeclarations
                %nterm <unique_ptr<MethodDeclaration>> InterfaceMemberDeclaration
                    %nterm <unique_ptr<MethodDeclaration>> AbstractMethodDeclaration
                        %nterm <vector<Modifier>> AbstractMethodModifiersOpt
                            %nterm <vector<Modifier>> AbstractMethodModifiers
                                %nterm <Modifier> AbstractMethodModifier
                        // MethodDeclarator


// QualifiedIdentifier (done)
%nterm <unique_ptr<QualifiedIdentifier>> QualifiedIdentifier
    %nterm <unique_ptr<Identifier>> Identifier

// Modifiers (done)
%nterm <vector<Modifier>> Modifiers
    %nterm <Modifier> Modifier

// Type (done)
%nterm <unique_ptr<Type>> Type
    %nterm <unique_ptr<Type>> PrimitiveType
        %nterm <PrimitiveType> IntegralType
        %nterm <PrimitiveType> BooleanType
    %nterm <unique_ptr<Type>> ReferenceType
        %nterm <unique_ptr<QualifiedIdentifier>> ClassOrInterfaceType

// VariableDeclarator (done)
%nterm <unique_ptr<VariableDeclarator>> VariableDeclarator
    %nterm <unique_ptr<Identifier>> VariableDeclaratorId
    %nterm <unique_ptr<Expression>> VariableInitializer

// Expression (done)
%nterm <unique_ptr<Expression>> Expression
    %nterm <unique_ptr<Expression>> AssignmentExpression
        %nterm <unique_ptr<Expression>> Assignment
            %nterm <unique_ptr<Expression>> LeftHandSide
                %nterm <unique_ptr<Expression>> FieldAccess
                    // Primary
                %nterm <unique_ptr<Expression>> ArrayAccess

    // ConditionalOrExpression (done)
    %nterm <unique_ptr<Expression>> ConditionalOrExpression
    %nterm <unique_ptr<Expression>> ConditionalAndExpression
    %nterm <unique_ptr<Expression>> InclusiveOrExpression
    %nterm <unique_ptr<Expression>> AndExpression
    %nterm <unique_ptr<Expression>> EqualityExpression
    %nterm <unique_ptr<Expression>> RelationalExpression
    %nterm <unique_ptr<Expression>> AdditiveExpression
    %nterm <unique_ptr<Expression>> MultiplicativeExpression
    %nterm <unique_ptr<Expression>> UnaryExpression
    %nterm <unique_ptr<Expression>> UnaryExpressionNotPlusMinus
    %nterm <unique_ptr<Expression>> CastExpression
    %nterm <unique_ptr<Expression>> PrimaryOrExpressionName

    // Primary (done)
    %nterm <unique_ptr<Expression>> Primary
        %nterm <unique_ptr<Expression>> PrimaryNoNewArray
            %nterm <unique_ptr<Literal>> Literal
            %nterm <unique_ptr<Expression>> ClassInstanceCreationExpression
                // Arguments
            %nterm <unique_ptr<Expression>> MethodInvocation
        %nterm <unique_ptr<Expression>> ArrayCreationExpression

// Statements (done)
%nterm <unique_ptr<Statement>> Statement
    %nterm <unique_ptr<Statement>> StatementWithoutTrailingSubstatement
        // Block
        %nterm <EmptyStatement> EmptyStatement
        %nterm <unique_ptr<ExpressionStatement>> ExpressionStatement
        %nterm <unique_ptr<Statement>> ReturnStatement
    %nterm <unique_ptr<Statement>> IfThenStatement
        // ParExpr, Statement
    %nterm <unique_ptr<Statement>> IfThenElseStatement
        %nterm <unique_ptr<Statement>> IfThenElseStatementNoShortIf
        // ParExpr, StatementNoShortIf, Statement
    %nterm <unique_ptr<Statement>> WhileStatement
        %nterm <unique_ptr<Statement>> WhileStatementNoShortIf
        // ParExpr, Statement
    %nterm <unique_ptr<Statement>> ForStatement
        %nterm <unique_ptr<Statement>> ForStatementNoShortIf
        %nterm <unique_ptr<Statement>> ForInitOpt
            %nterm <unique_ptr<Statement>> ForInit
                // (StatementExpression, LocalVariableDeclaration)
        %nterm <unique_ptr<Expression>> ExpressionOpt
            // (Expression)
        %nterm <unique_ptr<Statement>> ForUpdateOpt
            // (StatementExpression)
        // (Statement)

    // Common types for statements (done)
    %nterm <unique_ptr<Expression>> ParExpression
    %nterm <unique_ptr<Statement>> StatementNoShortIf
    %nterm <unique_ptr<ExpressionStatement>> StatementExpression

    // Block (done)
    %nterm <unique_ptr<Block>> Block
        %nterm <vector<Statement>> BlockStatementsOpt
        %nterm <vector<Statement>> BlockStatements
        %nterm <unique_ptr<Statement>> BlockStatement
            %nterm <unique_ptr<LocalVariableDeclaration>> LocalVariableDeclarationStatement
                %nterm <unique_ptr<LocalVariableDeclaration>> LocalVariableDeclaration
                    // (Type, VariableDeclarator)
            // Statement

// Arguments (done)
%nterm <vector<Expression>> Arguments
    %nterm <vector<Expression>> ArgumentListOpt
        %nterm <vector<Expression>> ArgumentList

/******************** END NONTERMINALS ********************/

%parse-param {CompilationUnit **root}

%{
#define SET_LOCATION(me, loc) \
    AddLocation()(*me, loc)
    // me->location = loc

#define MAKE_STACK_OBJ(me, type, constructor...) \
    me = type(constructor)

#define NEW_OBJ(type, constructor...) \
    make_unique<type>(constructor)

#define NEW_TYPE(type, isarray) \
    NEW_OBJ(Type, move(NEW_OBJ(NonArrayType, type)), isarray)

#define NEW_VARIANT_OBJ(outer_type, inner_type, inner_constructor...) \
    make_unique<outer_type>(in_place_type<inner_type>, inner_constructor)

#define MAKE_OBJ(me, type, constructor...) \
    me = make_unique<type>(constructor)

#define MAKE_VARIANT_OBJ(me, outer_type, inner_type, inner_constructor...) \
    me = make_unique<outer_type>(in_place_type<inner_type>, inner_constructor)

#define OBJ_TO_VARIANT(outer_type, obj) \
    NEW_OBJ(outer_type, move(obj))

#define MAKE_EXPRESSION_OBJ(me, inner_type, inner_constructor...) \
    MAKE_VARIANT_OBJ(me, Expression, inner_type, inner_constructor)

#define MAKE_LITERAL_OBJ(me, inner_type, inner_constructor...) \
    MAKE_VARIANT_OBJ(me, Literal, inner_type, inner_constructor)

#define MAKE_STATEMENT_OBJ(me, inner_type, inner_constructor...) \
    MAKE_VARIANT_OBJ(me, Statement, inner_type, inner_constructor)

#define MAKE_INFIX_OBJ(me, expr1, op, expr2) \
    MAKE_EXPRESSION_OBJ(me, InfixExpression, move( expr1 ), move( expr2 ), ( op ))

#define MAKE_PREFIX_OBJ(me, op, expr) \
    MAKE_EXPRESSION_OBJ(me, PrefixExpression, ( expr ), ( op ))

#define MAKE_CASTEXPR_OBJ(me, type, expr) \
    MAKE_EXPRESSION_OBJ(me, CastExpression, type, expr)

#define MAKE_QIEXPR_OBJ(me, qid) \
    me = OBJ_TO_VARIANT(Expression, qid)
    // MAKE_EXPRESSION_OBJ(me, QualifiedIdentifier, (*qid))

#define COPY_OBJ(me, you) \
    me = move(you)

#define MAKE_PAIR(me, obj1, obj2) \
    me = {move(obj1), move(obj2)}

#define EMPTY \
    (nullptr)

#define EMPTY_OBJ(type) \
    make_unique<type>(nullptr)

#define EMPTY_VECTOR(type) \
    (vector<type>())

#define EMPTY_MODIFIERS \
    (vector<Modifier>())

#define MAKE_CompilationUnit(me, args...) \
    MAKE_OBJ(me, CompilationUnit, args)

// expects (me, pair<vector<type1>, vector<type2>>, pair<type1, type2>)
#define MAKE_PAIRVECTORS(me, pair_of_vector, pair) \
    if ( pair.first ) { pair_of_vector.first.push_back(move( *pair.first )); } \
    if ( pair.second ) { pair_of_vector.second.push_back(move( *pair.second )); } \
    if ( &me != &pair_of_vector) { me = move(pair_of_vector); }

// expects (me, vector, item)
#define MAKE_VECTOR(me, vector, item) \
    vector.push_back(move(item)); \
    if ( &me != &vector ) { me = move(vector); }
%}

// Grammar
%%
%start Start;

/*---------------------- Packages ----------------------*/

Start:
    CompilationUnit {
        auto test = new CompilationUnit(nullptr, vector<QualifiedIdentifier>(), vector<QualifiedIdentifier>(), vector<ClassDeclaration>(), vector<InterfaceDeclaration>());
        *test = (move(*$1));
        *root = test;
        // *root = (AstNodeVariant*) test;
    }

// root = AstNodeVariant**
// $1 = unique_ptr<CompilationUnit>

CompilationUnit:
    PackageDeclaration ImportDeclarations TypeDeclarations
        { 
            MAKE_OBJ($$, CompilationUnit, move($1),   move($2.first), move($2.second),   move($3.first), move($3.second));
            SET_LOCATION($$, @$); 
        }
    | ImportDeclarations TypeDeclarations   // No PackageDeclaration
        {
            MAKE_OBJ($$, CompilationUnit, EMPTY,   move($1.first), move($1.second),   move($2.first), move($2.second));
            SET_LOCATION($$, @$); 
        }
    | PackageDeclaration TypeDeclarations   // No ImportDeclarations
        { MAKE_OBJ($$, CompilationUnit,
            move($1),                                                               // package
            EMPTY_VECTOR(QualifiedIdentifier), EMPTY_VECTOR(QualifiedIdentifier),   // import
            move($2.first), move($2.second));                                       // type
            SET_LOCATION($$, @$); 
            }
    | PackageDeclaration ImportDeclarations // No TypeDeclarations
        { MAKE_OBJ($$, CompilationUnit,
            move($1),
            move($2.first), move($2.second),
            EMPTY_VECTOR(ClassDeclaration), EMPTY_VECTOR(InterfaceDeclaration));
            SET_LOCATION($$, @$); 
            }
    | PackageDeclaration { MAKE_OBJ($$, CompilationUnit,
            move($1),
            EMPTY_VECTOR(QualifiedIdentifier), EMPTY_VECTOR(QualifiedIdentifier),
            EMPTY_VECTOR(ClassDeclaration), EMPTY_VECTOR(InterfaceDeclaration));
            SET_LOCATION($$, @$); 
        }
    | ImportDeclarations { MAKE_OBJ($$, CompilationUnit,
            EMPTY,
            move($1.first), move($1.second),
            EMPTY_VECTOR(ClassDeclaration), EMPTY_VECTOR(InterfaceDeclaration));
            SET_LOCATION($$, @$); 
        }
    | TypeDeclarations { MAKE_OBJ($$, CompilationUnit,
            EMPTY,
            EMPTY_VECTOR(QualifiedIdentifier), EMPTY_VECTOR(QualifiedIdentifier),
            move($1.first), move($1.second));
            SET_LOCATION($$, @$); 
        }
    | /* Empty */ { MAKE_OBJ($$, CompilationUnit,
            EMPTY,
            EMPTY_VECTOR(QualifiedIdentifier), EMPTY_VECTOR(QualifiedIdentifier),
            EMPTY_VECTOR(ClassDeclaration), EMPTY_VECTOR(InterfaceDeclaration));
            SET_LOCATION($$, @$); 
        }
    ;

PackageDeclaration:
    PACKAGE QualifiedIdentifier SEMI_COLON
        { COPY_OBJ($$, $2); SET_LOCATION($$, @$); }
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
	SingleTypeImportDeclaration { MAKE_PAIR($$, $1, EMPTY); }
	| TypeImportOnDemandDeclaration { MAKE_PAIR($$, EMPTY, $1); }
    ;

SingleTypeImportDeclaration:
    IMPORT QualifiedIdentifier SEMI_COLON // TypeName
        { COPY_OBJ($$, $2); SET_LOCATION($$, @$); }
    ;

TypeImportOnDemandDeclaration:
    IMPORT QualifiedIdentifier DOT ASTERISK SEMI_COLON // PackageOrTypeName
        { COPY_OBJ($$, $2); SET_LOCATION($$, @$); }
    ;

TypeDeclaration:
    ClassDeclaration { MAKE_PAIR($$, $1, EMPTY); }
    | InterfaceDeclaration { MAKE_PAIR($$, EMPTY, $1); }
    | SEMI_COLON { MAKE_PAIR($$, EMPTY, EMPTY); }
    ;

/*---------------------- Expressions ----------------------*/

Expression:
    AssignmentExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

AssignmentExpression:
    Assignment { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ConditionalOrExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

Assignment:
    LeftHandSide ASSIGNMENT AssignmentExpression { MAKE_EXPRESSION_OBJ($$, Assignment, move($1), move($3)); SET_LOCATION($$, @$); }

LeftHandSide:
    QualifiedIdentifier // ExpressionName
        { MAKE_OBJ($$, Expression, (Expression) move(*$1)); SET_LOCATION($$, @$); }
    | FieldAccess { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ArrayAccess { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

ConditionalOrExpression:
    ConditionalAndExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ConditionalOrExpression BOOLEAN_OR ConditionalAndExpression
        { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    ;

ConditionalAndExpression:
    InclusiveOrExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ConditionalAndExpression BOOLEAN_AND InclusiveOrExpression
        { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    ;

InclusiveOrExpression:
    AndExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | InclusiveOrExpression PIPE AndExpression { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }

AndExpression:
    EqualityExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | AndExpression AMPERSAND EqualityExpression { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }

EqualityExpression:
    RelationalExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | EqualityExpression BOOLEAN_EQUAL RelationalExpression  { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    | EqualityExpression NOT_EQUAL RelationalExpression { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    ;

RelationalExpression:
    AdditiveExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | RelationalExpression LESS_THAN AdditiveExpression
        { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    | RelationalExpression GREATER_THAN AdditiveExpression
        { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    | RelationalExpression LESS_THAN_EQUAL AdditiveExpression
        { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    | RelationalExpression GREATER_THAN_EQUAL AdditiveExpression
        { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    | RelationalExpression INSTANCEOF ReferenceType // ReferenceType
        { MAKE_EXPRESSION_OBJ($$, InstanceOfExpression, move($1), move($3)); SET_LOCATION($$, @$); }
    ;

AdditiveExpression:
    MultiplicativeExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | AdditiveExpression PLUS MultiplicativeExpression { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    | AdditiveExpression MINUS MultiplicativeExpression { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    ;

MultiplicativeExpression:
    UnaryExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | MultiplicativeExpression ASTERISK UnaryExpression { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    | MultiplicativeExpression DIVIDE UnaryExpression { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    | MultiplicativeExpression MODULO UnaryExpression { MAKE_INFIX_OBJ($$, $1, $2, $3); SET_LOCATION($$, @$); }
    ;

UnaryExpression:
    MINUS UnaryExpression {
            if (    // handle negative literals
                holds_alternative<Literal>(*$2) &&
                holds_alternative<int64_t>(get<Literal>(*$2))
            ) {
                get<int64_t>(get<Literal>(*$2)) *= -1;
            }
            MAKE_PREFIX_OBJ($$, PrefixOperator::MINUS, move($2));
            SET_LOCATION($$, @$); 
        }
    | UnaryExpressionNotPlusMinus { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

UnaryExpressionNotPlusMinus:
    NEGATE UnaryExpression { MAKE_PREFIX_OBJ($$, $1, move($2)); SET_LOCATION($$, @$); }
    | CastExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | PrimaryOrExpressionName { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

CastExpression: // Done this way to avoid conflicts
    OPENING_PAREN PrimitiveType CLOSING_PAREN UnaryExpression
        { MAKE_CASTEXPR_OBJ($$, move($2), move($4)); SET_LOCATION($$, @$); }
    | OPENING_PAREN Expression CLOSING_PAREN UnaryExpressionNotPlusMinus // Expression must be verified to be QualifiedIdentifier (ReferenceType no array)
        { MAKE_CASTEXPR_OBJ($$, NEW_TYPE(get<QualifiedIdentifier>(move(*$2)), false), move($4)); SET_LOCATION($$, @$); } // throws err if Expression not QI
    | OPENING_PAREN QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET CLOSING_PAREN UnaryExpressionNotPlusMinus // ReferenceType with array
        { MAKE_CASTEXPR_OBJ($$, NEW_TYPE(move(*$2), true), move($6)); SET_LOCATION($$, @$); }
    | OPENING_PAREN
        PrimitiveType OPENING_BRACKET CLOSING_BRACKET // ReferenceType as PrimitiveType with array
            CLOSING_PAREN UnaryExpressionNotPlusMinus
        { MAKE_CASTEXPR_OBJ($$, NEW_OBJ(Type, move($2->non_array_type), true), move($6)); SET_LOCATION($$, @$); }
    ;

PrimaryOrExpressionName:
    Primary { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | QualifiedIdentifier // ExpressionName
        { MAKE_OBJ($$, Expression, (Expression) move(*$1)); SET_LOCATION($$, @$); }
    ;

Primary:
    PrimaryNoNewArray { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ArrayCreationExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

ArrayCreationExpression:
    NEW PrimitiveType OPENING_BRACKET CLOSING_BRACKET
        { MAKE_EXPRESSION_OBJ($$, ArrayCreationExpression, move($2), EMPTY); SET_LOCATION($$, @$); }
    | NEW PrimitiveType OPENING_BRACKET Expression CLOSING_BRACKET
        { MAKE_EXPRESSION_OBJ($$, ArrayCreationExpression, move($2), move($4)); SET_LOCATION($$, @$); }
    | NEW QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET // TypeName
        { MAKE_EXPRESSION_OBJ($$, ArrayCreationExpression, NEW_TYPE(move(*$2), false), EMPTY); SET_LOCATION($$, @$); }
    | NEW QualifiedIdentifier OPENING_BRACKET Expression CLOSING_BRACKET // TypeName
        { MAKE_EXPRESSION_OBJ($$, ArrayCreationExpression, NEW_TYPE(move(*$2), false), move($4)); SET_LOCATION($$, @$); }
    ;

ClassInstanceCreationExpression:
    NEW QualifiedIdentifier OPENING_PAREN ArgumentListOpt CLOSING_PAREN
        { MAKE_EXPRESSION_OBJ($$, ClassInstanceCreationExpression, move($2), move($4)); SET_LOCATION($$, @$); }

PrimaryNoNewArray:
    Literal { MAKE_EXPRESSION_OBJ($$, Literal, move(*$1)); SET_LOCATION($$, @$); }
    | THIS { MAKE_EXPRESSION_OBJ($$, QualifiedThis, EMPTY); SET_LOCATION($$, @$); }
    | QualifiedIdentifier DOT THIS // ClassName     TODO: REMOVE
        { MAKE_EXPRESSION_OBJ($$, QualifiedThis, move($1)); SET_LOCATION($$, @$); }
    | OPENING_PAREN Expression CLOSING_PAREN
        { MAKE_EXPRESSION_OBJ($$, ParenthesizedExpression, move($2)); SET_LOCATION($$, @$); }
    | ClassInstanceCreationExpression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | FieldAccess { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | MethodInvocation { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ArrayAccess { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

Literal:
    INTEGER  { MAKE_LITERAL_OBJ($$, int64_t, $1); SET_LOCATION($$, @$); }
    | TRUE  { MAKE_LITERAL_OBJ($$, bool, $1); SET_LOCATION($$, @$); }
    | FALSE  { MAKE_LITERAL_OBJ($$, bool, $1); SET_LOCATION($$, @$); }
    | CHAR_LITERAL  { MAKE_LITERAL_OBJ($$, string, $1); SET_LOCATION($$, @$); }
    | STRING_LITERAL  { MAKE_LITERAL_OBJ($$, string, $1); SET_LOCATION($$, @$); }
    | NULL_TOKEN   { MAKE_LITERAL_OBJ($$, nullptr_t, $1); SET_LOCATION($$, @$); }
    ;

ArrayAccess:
    QualifiedIdentifier OPENING_BRACKET Expression CLOSING_BRACKET // ExpressionName
        { MAKE_EXPRESSION_OBJ($$, ArrayAccess, OBJ_TO_VARIANT(Expression, *$1), move($3)); SET_LOCATION($$, @$); }
    | PrimaryNoNewArray OPENING_BRACKET Expression CLOSING_BRACKET
        { MAKE_EXPRESSION_OBJ($$, ArrayAccess, move($1), move($3)); SET_LOCATION($$, @$); }
    ;

FieldAccess:
    Primary DOT Identifier
        { MAKE_EXPRESSION_OBJ($$, FieldAccess, move($1), move($3)); SET_LOCATION($$, @$); }
    ;

ArgumentListOpt:
    /* Empty - No arguments */ { MAKE_STACK_OBJ($$, vector<Expression>); }
    | ArgumentList { COPY_OBJ($$, $1); }
    ;

ArgumentList:
    Expression {
            MAKE_STACK_OBJ($$, vector<Expression>);
            MAKE_VECTOR($$, $$, *$1);
        }
    | ArgumentList COMMA Expression
        { MAKE_VECTOR($$, $1, *$3); }
    ;

Arguments:
    OPENING_PAREN ArgumentListOpt CLOSING_PAREN
        { COPY_OBJ($$, $2); }
    ;

MethodInvocation:
    QualifiedIdentifier Arguments // MethodName
        {
            if ( $1->identifiers.size() <= 1 ) { // Just method_name in QI - eg. toString()
                MAKE_EXPRESSION_OBJ($$, MethodInvocation, EMPTY, NEW_OBJ(Identifier, $1->identifiers.back()), move($2));
            } else { // QI has more than just method_name - eg. A.toString()
                MAKE_EXPRESSION_OBJ($$, MethodInvocation, NEW_VARIANT_OBJ(Expression, QualifiedIdentifier, $1->getQualifiedIdentifierWithoutLast().identifiers), NEW_OBJ(Identifier, $1->identifiers.back()), move($2));
            }
            SET_LOCATION($$, @$); 
        }
    | Primary DOT Identifier Arguments // Could we call this FieldAccess instead?
        { MAKE_EXPRESSION_OBJ($$, MethodInvocation, move($1), move($3), move($4)); SET_LOCATION($$, @$); }
    ;

Type:
    PrimitiveType { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ReferenceType { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

PrimitiveType:
    IntegralType { $$ = NEW_TYPE($1, false); SET_LOCATION($$, @$); }
    | BooleanType { $$ = NEW_TYPE($1, false); SET_LOCATION($$, @$); }
    ;

IntegralType:
    BYTE { $$ = $1; }
    | SHORT { $$ = $1; }
    | INT { $$ = $1; }
    | CHAR { $$ = $1; }
    ;

BooleanType:
    BOOLEAN { $$ = $1; }
    ;

ClassOrInterfaceType:
    QualifiedIdentifier // ClassType, InterfaceType -> TypeName
        { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }

ReferenceType: // Done this way to disallow multidimensional arrays
    ClassOrInterfaceType
        { MAKE_OBJ($$, Type, NEW_OBJ(NonArrayType, move(*$1)), false); SET_LOCATION($$, @$); }
    | ClassOrInterfaceType OPENING_BRACKET CLOSING_BRACKET
        { MAKE_OBJ($$, Type, NEW_OBJ(NonArrayType, move(*$1)), true); SET_LOCATION($$, @$); }
    | PrimitiveType OPENING_BRACKET CLOSING_BRACKET
        { COPY_OBJ($$, $1); $$->is_array = true; SET_LOCATION($$, @$); }
    ;

/*---------------------- Interfaces ----------------------*/

InterfaceDeclaration:
    InterfaceModifiersOpt INTERFACE Identifier ExtendsInterfacesOpt InterfaceBody
        { MAKE_OBJ($$, InterfaceDeclaration, move($1), move($3), move($4), move($5)); SET_LOCATION($$, @$); }
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
    /* Empty - optional */ { MAKE_STACK_OBJ($$, vector<Modifier>); }
    | Modifiers { $$ = $1; }
    ;

InterfaceType:
    QualifiedIdentifier { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

ExtendsInterfaces:
    EXTENDS InterfaceType { MAKE_STACK_OBJ($$, vector<QualifiedIdentifier>); MAKE_VECTOR($$, $$, *$2); }
    | ExtendsInterfaces COMMA InterfaceType { MAKE_VECTOR($$, $1, *$3); }
    ;

ExtendsInterfacesOpt:
    /* Empty - optional interface */ { MAKE_STACK_OBJ($$, vector<QualifiedIdentifier>); }
    | ExtendsInterfaces { COPY_OBJ($$, $1); }
    ;

InterfaceBody:
    OPENING_BRACE InterfaceMemberDeclarationsOpt CLOSING_BRACE
        { COPY_OBJ($$, $2); }
    ;

InterfaceMemberDeclarationsOpt:
    /* Empty - No interface body declarations */ { MAKE_STACK_OBJ($$, vector<MethodDeclaration>); }
    | InterfaceMemberDeclarations { $$ = move($1); }
    ;

InterfaceMemberDeclarations:
    InterfaceMemberDeclaration {
            MAKE_STACK_OBJ($$, vector<MethodDeclaration>);
            MAKE_VECTOR($$, $$, *$1);
        }
    | InterfaceMemberDeclarations InterfaceMemberDeclaration
        { MAKE_VECTOR($$, $1, *$2); }
    ;

InterfaceMemberDeclaration: // Nested types and interface constants not supported
    AbstractMethodDeclaration { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

AbstractMethodDeclaration:
    AbstractMethodModifiersOpt Type MethodDeclarator SEMI_COLON
        { MAKE_OBJ($$, MethodDeclaration, move($1), move($2), move($3.first), move($3.second), EMPTY); SET_LOCATION($$, @$); }
    | AbstractMethodModifiersOpt VOID MethodDeclarator SEMI_COLON
        { MAKE_OBJ($$, MethodDeclaration, move($1), NEW_TYPE($2, false), move($3.first), move($3.second), EMPTY); SET_LOCATION($$, @$); }
    ;

AbstractMethodModifiersOpt:
    /* Empty - optional */ { MAKE_STACK_OBJ($$, vector<Modifier>); }
    | AbstractMethodModifiers { $$ = $1; }
    ;

AbstractMethodModifiers:
    AbstractMethodModifier { 
            MAKE_STACK_OBJ($$, vector<Modifier>);
            MAKE_VECTOR($$, $$, $1);
        }
    | AbstractMethodModifiers AbstractMethodModifier
        { MAKE_VECTOR($$, $1, $2); }
    ;


/*---------------------- Classes ---------------------- JAGVIR START HERE */

// weeder: make sure at most one of implements and extends is present
// weeder: modifier can only be abstract, final, public
// weeder: max one of abstract/final
// weeder: must contain public?
ClassDeclaration:
    CLASS Identifier ExtendsOpt InterfacesOpt ClassBody
        { MAKE_OBJ($$, ClassDeclaration, EMPTY_VECTOR(Modifier), move($2), move($3), move($4), move($5.first), move($5.second)); SET_LOCATION($$, @$); }
    | Modifiers CLASS Identifier ExtendsOpt InterfacesOpt ClassBody
        { MAKE_OBJ($$, ClassDeclaration, move($1), move($3), move($4), move($5), move($6.first), move($6.second)); SET_LOCATION($$, @$); }
    ;

/* Class interfaces */
InterfacesOpt:
    /* Empty - No implements */ { MAKE_STACK_OBJ($$, vector<QualifiedIdentifier>); }
    | Interfaces { $$ = $1; }
    ;

Interfaces:
    IMPLEMENTS InterfaceTypeList { COPY_OBJ($$, $2); }
    ;

InterfaceTypeList:
    InterfaceType { MAKE_STACK_OBJ($$, vector<QualifiedIdentifier>); MAKE_VECTOR($$, $$, *$1); }
    | InterfaceTypeList COMMA InterfaceType { MAKE_VECTOR($$, $1, *$3); }
    ;

/* Class Extends */
ExtendsOpt:
    /* Empty - No extends */ { $$ = EMPTY; }
    | EXTENDS QualifiedIdentifier { COPY_OBJ($$, $2); SET_LOCATION($$, @$); } // ClassType
    ;

/* Class body */
ClassBodyDeclarationsOpt:
    /* Empty */ { MAKE_STACK_OBJ($$, pair<vector<FieldDeclaration> COMMA vector<MethodDeclaration>>); }
    | ClassBodyDeclarations { COPY_OBJ($$, $1); }
    ;

ClassBodyDeclarations:
    ClassBodyDeclaration {
            MAKE_STACK_OBJ($$, pair<vector<FieldDeclaration> COMMA vector<MethodDeclaration>>);
            MAKE_PAIRVECTORS($$, $$, $1);
        }
    | ClassBodyDeclarations ClassBodyDeclaration { MAKE_PAIRVECTORS($$, $1, $2); }
    ;

ClassBodyDeclaration:
    ClassMemberDeclaration { COPY_OBJ($$, $1); }
    ;
    // ensure there is at least one constructor
    // | InstanceInitializer: omitted from joos
    // | StaticInitializer: omitted from joos

ClassMemberDeclaration:
    FieldDeclaration { MAKE_PAIR($$, $1, EMPTY); }
    | MethodDeclaration { MAKE_PAIR($$, EMPTY, $1); }
    | SEMI_COLON { MAKE_PAIR($$, EMPTY, EMPTY); }
    ;
    // | ClassDeclaration: omitted for joos
    // | InterfaceDeclaration: NOT SURE IF THIS SHOULD BE OMITTED FOR JOOS

ClassBody:
    OPENING_BRACE ClassBodyDeclarationsOpt CLOSING_BRACE { COPY_OBJ($$, $2); }
    ;

/* Fields */
// Only one variable declaration is allowed at a time
FieldDeclaration:
    Type VariableDeclarator SEMI_COLON { MAKE_OBJ($$, FieldDeclaration, EMPTY_VECTOR(Modifier), move($1), move($2)); SET_LOCATION($$, @$); }
    | Modifiers Type VariableDeclarator SEMI_COLON { MAKE_OBJ($$, FieldDeclaration, move($1), move($2), move($3)); SET_LOCATION($$, @$); }
    ;

VariableInitializer:
    Expression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

/* Methods */
// weeder: methodbody exists if neither abstract nor native
MethodDeclaration: // One of these must be constructor
    MethodHeader MethodBody { COPY_OBJ($$, $1); COPY_OBJ($$->body, $2); SET_LOCATION($$, @$); }
    ;

// weeder: allow static native int m(int)
// weeder: see A1 specs for weeding modifiers
MethodHeader:
    Type MethodDeclarator { MAKE_OBJ($$, MethodDeclaration, EMPTY_VECTOR(Modifier), move($1), move($2.first), move($2.second), EMPTY); SET_LOCATION($$, @$); }
    | Modifiers Type MethodDeclarator { MAKE_OBJ($$, MethodDeclaration, move($1), move($2), move($3.first), move($3.second), EMPTY); SET_LOCATION($$, @$); }
    | VOID MethodDeclarator { MAKE_OBJ($$, MethodDeclaration, EMPTY_VECTOR(Modifier), NEW_TYPE($1, false), move($2.first), move($2.second), EMPTY); SET_LOCATION($$, @$); }
    | Modifiers VOID MethodDeclarator { MAKE_OBJ($$, MethodDeclaration, move($1), NEW_TYPE($2, false), move($3.first), move($3.second), EMPTY); SET_LOCATION($$, @$); }
    | Modifiers MethodDeclarator // Represents constructor, todo weeding: reject if identifier is not class name
        { MAKE_OBJ($$, MethodDeclaration, move($1), EMPTY, move($2.first), move($2.second), EMPTY); SET_LOCATION($$, @$); }
    ;

MethodDeclarator:
    Identifier OPENING_PAREN FormalParameterListOpt CLOSING_PAREN
        { MAKE_PAIR($$, $1, $3); }
    ;

MethodBody:
    SEMI_COLON { $$ = EMPTY; }
    | Block { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

/* Formal parameters */
FormalParameterListOpt:
    /* EMPTY */ { MAKE_STACK_OBJ($$, vector<FormalParameter>); }
    | FormalParameterList { $$ = move($1); }
    ;

FormalParameterList:
    FormalParameter { MAKE_STACK_OBJ($$, vector<FormalParameter>); MAKE_VECTOR($$, $$, *$1); }
    | FormalParameterList COMMA FormalParameter { MAKE_VECTOR($$, $1, *$3); }
    ;
/*-----------------------*/

AbstractMethodModifier:
    PUBLIC { $$ = $1; }
    | ABSTRACT { $$ = $1; }
    ;

FormalParameter:
	Type VariableDeclaratorId { MAKE_OBJ($$, FormalParameter, move($1), move($2)); SET_LOCATION($$, @$); }
    ;

VariableDeclaratorId:
    Identifier { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    // | Identifier OPENING_BRACKET CLOSING_BRACKET { MAKE_NODE($$, symbol_kind::S_VariableDeclaratorId, $1, $2, $3); }
    ;

/*---------------------- Statements ----------------------*/

Statement:
    StatementWithoutTrailingSubstatement { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | IfThenStatement { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | IfThenElseStatement { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | WhileStatement { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ForStatement { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

StatementWithoutTrailingSubstatement:
    Block { MAKE_STATEMENT_OBJ($$, Block, EMPTY_VECTOR(LocalVariableDeclaration), move($1->statements)); SET_LOCATION($$, @$); }
    | EmptyStatement { MAKE_STATEMENT_OBJ($$, EmptyStatement, $1); SET_LOCATION($$, @$); }
    | ExpressionStatement { MAKE_OBJ($$, Statement, move(*$1)); SET_LOCATION($$, @$); }
    | ReturnStatement { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

ExpressionStatement:
    StatementExpression SEMI_COLON { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

StatementExpression:
    Assignment
        { $$ = OBJ_TO_VARIANT(ExpressionStatement, get<Assignment>(move(*$1))); SET_LOCATION($$, @$); }
    | MethodInvocation
        { $$ = OBJ_TO_VARIANT(ExpressionStatement, get<MethodInvocation>(move(*$1))); SET_LOCATION($$, @$); }
    | ClassInstanceCreationExpression
        { $$ = OBJ_TO_VARIANT(ExpressionStatement, get<ClassInstanceCreationExpression>(move(*$1))); SET_LOCATION($$, @$); }
    ;

StatementNoShortIf:
    StatementWithoutTrailingSubstatement { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | IfThenElseStatementNoShortIf { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | WhileStatementNoShortIf { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    | ForStatementNoShortIf { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

EmptyStatement:
    SEMI_COLON { MAKE_STACK_OBJ($$, EmptyStatement); }
	;

IfThenStatement:
	IF ParExpression Statement { MAKE_STATEMENT_OBJ($$, IfThenStatement, move($2), move($3)); SET_LOCATION($$, @$); }
    ;

IfThenElseStatement:
	IF ParExpression StatementNoShortIf ELSE Statement { MAKE_STATEMENT_OBJ($$, IfThenElseStatement, move($2), move($3), move($5)); SET_LOCATION($$, @$); }
    ;

IfThenElseStatementNoShortIf:
	IF ParExpression StatementNoShortIf ELSE StatementNoShortIf { MAKE_STATEMENT_OBJ($$, IfThenElseStatement, move($2), move($3), move($5)); SET_LOCATION($$, @$); }
    ;

WhileStatement:
	  WHILE ParExpression Statement { MAKE_STATEMENT_OBJ($$, WhileStatement, move($2), move($3)); SET_LOCATION($$, @$); }
    ;

WhileStatementNoShortIf:
	  WHILE ParExpression StatementNoShortIf { MAKE_STATEMENT_OBJ($$, WhileStatement, move($2), move($3)); SET_LOCATION($$, @$); }
    ;

ForStatement:
	  FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN Statement
          { MAKE_STATEMENT_OBJ($$, ForStatement, move($3), move($5), move($7), move($9)); SET_LOCATION($$, @$); }
    ;

ForStatementNoShortIf:
	  FOR OPENING_PAREN ForInitOpt SEMI_COLON ExpressionOpt SEMI_COLON ForUpdateOpt CLOSING_PAREN StatementNoShortIf
          { MAKE_STATEMENT_OBJ($$, ForStatement, move($3), move($5), move($7), move($9)); SET_LOCATION($$, @$); }
    ;
		
ForInitOpt:
    /* No init */ { $$ = EMPTY; }
    | ForInit { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

ForInit:
    StatementExpression { $$ = OBJ_TO_VARIANT(Statement, *$1); SET_LOCATION($$, @$); }
    | LocalVariableDeclaration { $$ = OBJ_TO_VARIANT(Statement, *$1); SET_LOCATION($$, @$); }
    ;

ForUpdateOpt:
    /* Empty - no update */ { $$ = EMPTY; }
    | StatementExpression { $$ = OBJ_TO_VARIANT(Statement, *$1); SET_LOCATION($$, @$); }
    ;

ExpressionOpt:
    /* no expression */ { $$ = EMPTY; }
    | Expression { COPY_OBJ($$, $1); SET_LOCATION($$, @$); }
    ;

ReturnStatement:
    RETURN ExpressionOpt SEMI_COLON { MAKE_STATEMENT_OBJ($$, ReturnStatement, move($2)); SET_LOCATION($$, @$); }
    ;

ParExpression:
    OPENING_PAREN Expression CLOSING_PAREN { MAKE_EXPRESSION_OBJ($$, ParenthesizedExpression, move($2)); SET_LOCATION($$, @$); }
    ;

QualifiedIdentifier:
    Identifier
        {
            MAKE_OBJ($$, QualifiedIdentifier, EMPTY_VECTOR(Identifier));
            $$->identifiers.push_back(move(*$1));
            SET_LOCATION($$, @$);
        }
    | QualifiedIdentifier DOT Identifier
        { 
            COPY_OBJ($$, $1);
            $$->identifiers.push_back(move(*$3));
        }
    ;

Identifier:
    IDENTIFIER { MAKE_OBJ($$, Identifier, $1); SET_LOCATION($$, @$); }
    ;

// Delay Type reduce due to conflict
LocalVariableDeclaration:
    // Type VariableDeclarators
    QualifiedIdentifier VariableDeclarator // ClassOrInterfaceType VariableDeclarators
        { MAKE_OBJ($$, LocalVariableDeclaration, NEW_TYPE(move(*$1), false), move($2)); SET_LOCATION($$, @$); }
    | QualifiedIdentifier OPENING_BRACKET CLOSING_BRACKET VariableDeclarator // ClassOrInterfaceTypeArray VariableDeclarators
        { MAKE_OBJ($$, LocalVariableDeclaration, NEW_TYPE(move(*$1), true), move($4)); SET_LOCATION($$, @$); }
    | PrimitiveType OPENING_BRACKET CLOSING_BRACKET VariableDeclarator // PrimitiveArray VariableDeclarators
        { $1->is_array = true; MAKE_OBJ($$, LocalVariableDeclaration, move($1), move($4)); SET_LOCATION($$, @$); }
    | PrimitiveType VariableDeclarator
        { MAKE_OBJ($$, LocalVariableDeclaration, move($1), move($2)); SET_LOCATION($$, @$); }
    ;

Block:
    OPENING_BRACE BlockStatementsOpt CLOSING_BRACE { MAKE_OBJ($$, Block, EMPTY_VECTOR(LocalVariableDeclaration), move($2)); SET_LOCATION($$, @$); }
    ;

BlockStatementsOpt:
    /* Empty - represents zero BlockStatements */ { MAKE_STACK_OBJ($$, vector<Statement>); }
    | BlockStatements { COPY_OBJ($$, $1); }
    ;

BlockStatements:
    BlockStatement {
            MAKE_STACK_OBJ($$, vector<Statement>);
            MAKE_VECTOR($$, $$, *$1);
        }
    | BlockStatements BlockStatement { MAKE_VECTOR($$, $1, *$2); }
    ;

BlockStatement:
    LocalVariableDeclarationStatement  { $$ = OBJ_TO_VARIANT(Statement, *$1); }
    | Statement { COPY_OBJ($$, $1); }
    ;

LocalVariableDeclarationStatement:
    LocalVariableDeclaration SEMI_COLON { COPY_OBJ($$, $1); }
    ;

VariableDeclarator:
    VariableDeclaratorId { MAKE_OBJ($$, VariableDeclarator, move($1), EMPTY); }
    | VariableDeclaratorId ASSIGNMENT VariableInitializer { MAKE_OBJ($$, VariableDeclarator, move($1), move($3)); }
    ;

// -------------------------------------------------------------

%%

void yy::parser::error (const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << '\n';
}
