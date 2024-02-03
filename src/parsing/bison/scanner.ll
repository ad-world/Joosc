%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <string>
# include "driver.h"
# include "parser.hh"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
# undef yywrap
# define yywrap() 1

// Pacify warnings in yy_init_buffer (observed with Flex 2.6.4)
// and GCC 7.3.0.
#if defined __GNUC__ && 7 <= __GNUC__
# pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
%}

%option noyywrap nounput batch noinput
/* %option debug */

Whitespace      [ \b\t\f\r]
Letter          [a-zA-Z]
Digit           [0-9]
Identifier      {Letter}({Digit}|{Letter}|_)*
Integer         0|[1-9]{Digit}*
Float           {Digit}+"."{Digit}+
Ascii           [ -!]|[#-&]|[(-~]
OctalEscape     \\[0-3]?[0-7]?[0-7]?
Escape          \\[tbnrf\\]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}
%%
%{
  // A handy shortcut to the location held by the driver.
  yy::location& loc = drv.location;
  // Code run each time yylex is called.
  loc.step ();
%}
goto|volatile|super|long|float|double|throws|throw|try|catch|finally|do|switch|break|continue|synchronized|"--"  {
             throw yy::parser::syntax_error
               (loc, "invalid token: '" + std::string(yytext) + "' is not a valid joosc token, only a Java token");
}

if    return yy::parser::make_IF(new AstNode(yy::parser::symbol_kind::S_IF), loc);
while     return yy::parser::make_WHILE(new AstNode(yy::parser::symbol_kind::S_WHILE), loc);
for     return yy::parser::make_FOR(new AstNode(yy::parser::symbol_kind::S_FOR), loc);
else    return yy::parser::make_ELSE(new AstNode(yy::parser::symbol_kind::S_ELSE), loc);
extends     return yy::parser::make_EXTENDS(new AstNode(yy::parser::symbol_kind::S_EXTENDS), loc);
new     return yy::parser::make_NEW(new AstNode(yy::parser::symbol_kind::S_NEW), loc);
public    return yy::parser::make_PUBLIC(new AstNode(yy::parser::symbol_kind::S_PUBLIC), loc);
implements    return yy::parser::make_IMPLEMENTS(new AstNode(yy::parser::symbol_kind::S_IMPLEMENTS), loc);
protected     return yy::parser::make_PROTECTED(new AstNode(yy::parser::symbol_kind::S_PROTECTED), loc);
static    return yy::parser::make_STATIC(new AstNode(yy::parser::symbol_kind::S_STATIC), loc);
abstract      return yy::parser::make_ABSTRACT(new AstNode(yy::parser::symbol_kind::S_ABSTRACT), loc);
this      return yy::parser::make_THIS(new AstNode(yy::parser::symbol_kind::S_THIS), loc);
void      return yy::parser::make_VOID(new AstNode(yy::parser::symbol_kind::S_VOID), loc);
final     return yy::parser::make_FINAL(new AstNode(yy::parser::symbol_kind::S_FINAL), loc);
import      return yy::parser::make_IMPORT(new AstNode(yy::parser::symbol_kind::S_IMPORT), loc);
class      return yy::parser::make_CLASS(new AstNode(yy::parser::symbol_kind::S_CLASS), loc);
package     return yy::parser::make_PACKAGE(new AstNode(yy::parser::symbol_kind::S_PACKAGE), loc);
interface     return yy::parser::make_INTERFACE(new AstNode(yy::parser::symbol_kind::S_INTERFACE), loc);
native    return yy::parser::make_NATIVE(new AstNode(yy::parser::symbol_kind::S_NATIVE), loc);
return    return yy::parser::make_RETURN(new AstNode(yy::parser::symbol_kind::S_RETURN), loc);
instanceof return yy::parser::make_INSTANCEOF(new AstNode(yy::parser::symbol_kind::S_INSTANCEOF), loc);
"{"       return yy::parser::make_OPENING_BRACE(new AstNode(yy::parser::symbol_kind::S_OPENING_BRACE), loc);
"}"       return yy::parser::make_CLOSING_BRACE(new AstNode(yy::parser::symbol_kind::S_CLOSING_BRACE), loc);
"["       return yy::parser::make_OPENING_BRACKET(new AstNode(yy::parser::symbol_kind::S_OPENING_BRACKET), loc);
"]"       return yy::parser::make_CLOSING_BRACKET(new AstNode(yy::parser::symbol_kind::S_CLOSING_BRACKET), loc);
"("       return yy::parser::make_OPENING_PAREN(new AstNode(yy::parser::symbol_kind::S_OPENING_PAREN), loc);
")"       return yy::parser::make_CLOSING_PAREN(new AstNode(yy::parser::symbol_kind::S_CLOSING_PAREN), loc);
";"       return yy::parser::make_SEMI_COLON(new AstNode(yy::parser::symbol_kind::S_SEMI_COLON), loc);
"."     return yy::parser::make_DOT(new AstNode(yy::parser::symbol_kind::S_DOT), loc);
"="     return yy::parser::make_ASSIGNMENT(new AstNode(yy::parser::symbol_kind::S_ASSIGNMENT), loc);

%{ // Types %}
int     return yy::parser::make_INT(new AstNode(yy::parser::symbol_kind::S_INT), loc);
boolean    return yy::parser::make_BOOLEAN(new AstNode(yy::parser::symbol_kind::S_BOOLEAN), loc);
char    return yy::parser::make_CHAR(new AstNode(yy::parser::symbol_kind::S_CHAR), loc); 
byte    return yy::parser::make_BYTE(new AstNode(yy::parser::symbol_kind::S_BYTE), loc);
short   return yy::parser::make_SHORT(new AstNode(yy::parser::symbol_kind::S_SHORT), loc);

%{ // Literals %}
true                return yy::parser::make_TRUE(new AstNode(yy::parser::symbol_kind::S_TRUE), loc);
false               return yy::parser::make_FALSE(new AstNode(yy::parser::symbol_kind::S_FALSE), loc);
\"({Ascii}|{OctalEscape}|{Escape}|\\\"|\')*\"  {
    return yy::parser::make_STRING_LITERAL(new AstNode(yy::parser::symbol_kind::S_STRING_LITERAL, ((std::string) yytext).substr(1, ((std::string) yytext).size() - 2)), loc);
}
{Integer}           return yy::parser::make_INTEGER(new AstNode(yy::parser::symbol_kind::S_INTEGER, stol(yytext)), loc);
null                return yy::parser::make_NULL_TOKEN(new AstNode(yy::parser::symbol_kind::S_NULL_TOKEN), loc);
\'({Ascii}|{OctalEscape}|{Escape}|\"|\\\')\'         {
    return yy::parser::make_CHAR_LITERAL(new AstNode(yy::parser::symbol_kind::S_CHAR_LITERAL, ((std::string) yytext).substr(1, ((std::string) yytext).size() - 2)), loc);
}

%{ // Comments %}
"//".*     { }
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]      { }
\/\*\*.*\*\/    { } 

%{ // Operators %}
"!"     return yy::parser::make_NEGATE(new AstNode(yy::parser::symbol_kind::S_NEGATE), loc);
"+"     return yy::parser::make_PLUS(new AstNode(yy::parser::symbol_kind::S_PLUS), loc);
"-"     return yy::parser::make_MINUS(new AstNode(yy::parser::symbol_kind::S_MINUS), loc);
"*"     return yy::parser::make_ASTERISK(new AstNode(yy::parser::symbol_kind::S_ASTERISK), loc);
"/"     return yy::parser::make_DIVIDE(new AstNode(yy::parser::symbol_kind::S_DIVIDE), loc);
"%"     return yy::parser::make_MODULO(new AstNode(yy::parser::symbol_kind::S_MODULO), loc);
"<"     return yy::parser::make_LESS_THAN(new AstNode(yy::parser::symbol_kind::S_LESS_THAN), loc);
">"     return yy::parser::make_GREATER_THAN(new AstNode(yy::parser::symbol_kind::S_GREATER_THAN), loc);
"<="    return yy::parser::make_LESS_THAN_EQUAL(new AstNode(yy::parser::symbol_kind::S_LESS_THAN_EQUAL), loc);
">="    return yy::parser::make_GREATER_THAN_EQUAL(new AstNode(yy::parser::symbol_kind::S_GREATER_THAN_EQUAL), loc);
"=="    return yy::parser::make_BOOLEAN_EQUAL(new AstNode(yy::parser::symbol_kind::S_BOOLEAN_EQUAL), loc);
"!="    return yy::parser::make_NOT_EQUAL(new AstNode(yy::parser::symbol_kind::S_NOT_EQUAL), loc);
"&&"    return yy::parser::make_BOOLEAN_AND(new AstNode(yy::parser::symbol_kind::S_BOOLEAN_AND), loc);
"&"     return yy::parser::make_AMPERSAND(new AstNode(yy::parser::symbol_kind::S_AMPERSAND), loc);
"||"    return yy::parser::make_BOOLEAN_OR(new AstNode(yy::parser::symbol_kind::S_BOOLEAN_OR), loc);
"|"     return yy::parser::make_PIPE(new AstNode(yy::parser::symbol_kind::S_PIPE), loc);

{Whitespace}+      loc.step ();
{Identifier}       return yy::parser::make_IDENTIFIER(new AstNode(yy::parser::symbol_kind::S_IDENTIFIER, yytext), loc);

.          {
             throw yy::parser::syntax_error
               (loc, "invalid character: " + std::string(yytext));
}
<<EOF>>    return yy::parser::make_EOF (new AstNode(yy::parser::symbol_kind::S_YYEOF),loc);
[\n]+      loc.lines (yyleng); loc.step ();
%%

void Driver::scan_begin() {
  yy_flex_debug = trace_scanning;
  if (file.empty()|| file == "-")
    yyin = stdin;
  else if (!(yyin = fopen (file.c_str (), "r")))
    {
      std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
      exit (EXIT_FAILURE);
    }
}

void Driver::scan_end() {
  fclose(yyin);
}

