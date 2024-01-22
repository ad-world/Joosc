%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.2"
%defines
%define api.parser.class { Parser }

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { JooscParser }

%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>

    using namespace std;
}

%code {
  int yylex (void) {
    return 0;
  }
}

%locations
%define parse.trace
%define parse.error verbose

// %define api.token.prefix {TOKEN_}

%%
%start program;
program: %empty;
%%

// Bison expects us to provide implementation - otherwise linker complains
void JooscParser::Parser::error (const location_type& l, const std::string& m) {
  std::cerr << l << ": " << m << '\n';
}
