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

%define api.token.prefix {TOK_}
%token <std::string> IDENTIFIER
%token END 0

%type <ParseTreeNode> program

// Grammar
%%
%start program;

program:
  IDENTIFIER {
    driver.parse_tree = ParseTreeNode(ParseTreeNode_t::Identifier); 
  }
%%

void yy::parser::error (const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << '\n';
}
