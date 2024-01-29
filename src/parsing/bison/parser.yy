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

%type <ParseTreeNode> program

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

// END OF FILE TOKEN
%token EOF 0
/*****************************************************************************/


// Grammar
%%
%start program;

program:
  IDENTIFIER { // Placeholder example
    driver.parse_tree = ParseTreeNode(ParseTreeNode_t::Identifier, 1);
    auto child = ParseTreeNode(ParseTreeNode_t::Identifier);
    driver.parse_tree.addChild(child);
  }
%%

void yy::parser::error (const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << '\n';
}
