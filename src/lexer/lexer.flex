%{
#include <iostream>
#include "token.h"
%}

%option noyywrap
%option yylineno

Whitespace [ \t\f\r\n]
Letter [a-zA-Z]
Digit [0-9]
Identifier {Letter}({Digit}|{Letter}|_)*
Integer "0" | "-"?[1-9]{Digit}*
Float {Digit}+"."{Digit}+

%%
"if"                                { return Token(IF, yytext(), yylineno, yyleng); }
"while"                             { return Token(WHILE, yytext(), yylineno, yyleng); }
"for"                               { return Token(FOR, yytext(), yylineno, yyleng); }
"extends"                           { return Token(EXTENDS, yytext(), yylineno, yyleng); }
"implements"                        { return Token(IMPLEMENTS, yytext(), yylineno, yyleng); }
"public"                            { return Token(PUBLIC, yytext(), yylineno, yyleng); }
"protected"                         { return Token(PROTECTED, yytext(), yylineno, yyleng); }
"private"                           { return Token(PRIVATE, yytext(), yylineno, yyleng); }
"static"                            { return Token(STATIC, yytext(), yylineno, yyleng); }
"abstract"                          { return Token(ABSTRACT, yytext(), yylineno, yyleng); }
"this"                              { return Token(THIS, yytext(), yylineno, yyleng); }
"void"                              { return Token(VOID, yytext(), yylineno, yyleng); }
"final"                             { return Token(FINAL, yytext(), yylineno, yyleng); }
"import"                            { return Token(IMPORT, yytext(), yylineno, yyleng); }
"class"                             { return Token(CLASS, yytext(), yylineno, yyleng); }
{Integer}                           { return Token(INTEGER, std::stoi(yytext()), yylineno, yyleng); }
{Identifier}("."{Identifier})*      { return Token(IMPORTED_PACKAGE, yytext(), yylineno, yyleng); }
{Identifier}("."{Identifier})+      { return Token(COMPOUND_NAME, yytext(), yylineno, yyleng); }
"package"                           { return Token(PACKAGE, yytext(), yylineno, yyleng); }
"interface"                         { return Token(INTERFACE, yytext(), yylineno, yyleng); }
"{"                                 { return Token(OPENING_BRACE, yytext(), yylineno, yyleng); }
"}"                                 { return Token(CLOSING_BRACE, yytext(), yylineno, yyleng); }
"("                                 { return Token(OPENING_PAREN, yytext(), yylineno, yyleng); }
")"                                 { return Token(CLOSING_PAREN, yytext(), yylineno, yyleng); }
"["                                 { return Token(OPENING_BRACKET, yytext(), yylineno, yyleng); }
"]"                                 { return Token(CLOSING_BRACKET, yytext(), yylineno, yyleng); }
";"                                 { return Token(SEMI_COLON, yytext(), yylineno, yyleng); }
"."                                 { return Token(DOT, yytext(), yylineno, yyleng); }
"native"                            { return Token(NATIVE, yytext(), yylineno, yyleng); }
"return"                            { return Token(RETURN, yytext(), yylineno, yyleng); }
"int"                               { return Token(INT, yytext(), yylineno, yyleng); }
"boolean"                           { return Token(BOOLEAN, yytext(), yylineno, yyleng); }
"char"                              { return Token(CHAR, yytext(), yylineno, yyleng); }
"byte"                              { return Token(BYTE, yytext(), yylineno, yyleng); }
"short"                             { return Token(SHORT, yytext(), yylineno, yyleng); }
"array"                             { return Token(ARRAY, yytext(), yylineno, yyleng); }
"true"                              { return Token(TRUE, yytext(), yylineno, yyleng); }
"false"                             { return Token(FALSE, yytext(), yylineno, yyleng); }
\"([^"\\]|\\.)*\"                   { return Token(STRING_LITERAL, yytext(), yylineno, yyleng); }
"null"                              { return Token(NULL_TOKEN, yytext(), yylineno, yyleng); }
\'([^\'\\]|\\.)*\'                  { return Token(CHAR_LITERAL, yytext(), yylineno, yyleng); }
"/*"[^*]*"*/"                       { return Token(MULTI_LINE_COMMENT, yytext(), yylineno, yyleng); }
"//".*                              { return Token(SINGLE_LINE_COMMENT, yytext(), yylineno, yyleng); }
"/**"[^*]*"*/"                      { return Token(JAVADOC_COMMENT, yytext(), yylineno, yyleng); }
"!="                                { return Token(NOT_EQUAL, yytext(), yylineno, yyleng); }
"!"                                 { return Token(NEGATE, yytext(), yylineno, yyleng); }
"+"                                 { return Token(PLUS, yytext(), yylineno, yyleng); }
"-"                                 { return Token(MINUS, yytext(), yylineno, yyleng); }
"*"                                 { return Token(MULTIPLY, yytext(), yylineno, yyleng); }
"/"                                 { return Token(DIVIDE, yytext(), yylineno, yyleng); }
"%"                                 { return Token(MODULO, yytext(), yylineno, yyleng); }
"<="                                { return Token(LESS_THAN_EQUAL, yytext(), yylineno, yyleng); }
">="                                { return Token(GREATER_THAN_EQUAL, yytext(), yylineno, yyleng); }
">"                                 { return Token(GREATER_THAN, yytext(), yylineno, yyleng); }
"<"                                 { return Token(LESS_THAN, yytext(), yylineno, yyleng); }
"=="                                { return Token(BOOLEAN_EQUAL, yytext(), yylineno, yyleng); }
"&"                                 { return Token(AMPERSAND, yytext(), yylineno, yyleng); }
"|"                                 { return Token(PIPE, yytext(), yylineno, yyleng); }
"="                                 { return Token(ASSIGNMENT, yytext(), yylineno, yyleng); }
[a-zA-Z_][a-zA-Z0-9_]*              { return Token(IDENTIFIER, yytext(), yylineno, yyleng); }

[ \t\n]+            ; // Ignore whitespace
.                                   { std::cerr << "Error: Unrecognized character '" << yytext << "' at line " << yylineno << ", column " << yyleng << std::endl; }
%%

int main() {
    yylex();
    return 0;
}