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
Ascii           [ -~]

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
volatile|super|long|float|double|throws|throw|try|catch|finally|do|switch|break|continue|synchronized  {
             throw yy::parser::syntax_error
               (loc, "invalid token: '" + std::string(yytext) + "' is not a valid joosc token, only a Java token");
}

if    return yy::parser::make_IF(new AstNode("IF"), loc);
while     return yy::parser::make_WHILE(new AstNode("WHILE"), loc);
for     return yy::parser::make_FOR(new AstNode("FOR"), loc);
else    return yy::parser::make_ELSE(new AstNode("ELSE"), loc);
extends     return yy::parser::make_EXTENDS(new AstNode("EXTENDS"), loc);
new     return yy::parser::make_NEW(new AstNode("NEW"), loc);
public    return yy::parser::make_PUBLIC(new AstNode("PUBLIC"), loc);
implements    return yy::parser::make_IMPLEMENTS(new AstNode("IMPLEMENTS"), loc);
protected     return yy::parser::make_PROTECTED(new AstNode("PROTECTED"), loc);
private     return yy::parser::make_PRIVATE(new AstNode("PRIVATE"), loc);
static    return yy::parser::make_STATIC(new AstNode("STATIC"), loc);
abstract      return yy::parser::make_ABSTRACT(new AstNode("ABSTRACT"), loc);
this      return yy::parser::make_THIS(new AstNode("THIS"), loc);
void      return yy::parser::make_VOID(new AstNode("VOID"), loc);
final     return yy::parser::make_FINAL(new AstNode("FINAL"), loc);
import      return yy::parser::make_IMPORT(new AstNode("IMPORT"), loc);
class      return yy::parser::make_CLASS(new AstNode("CLASS"), loc);
package     return yy::parser::make_PACKAGE(new AstNode("PACKAGE"), loc);
interface     return yy::parser::make_INTERFACE(new AstNode("INTERFACE"), loc);
native    return yy::parser::make_NATIVE(new AstNode("NATIVE"), loc);
return    return yy::parser::make_RETURN(new AstNode("RETURN"), loc);
instanceof return yy::parser::make_INSTANCEOF(new AstNode("INSTANCEOF"), loc);
"{"       return yy::parser::make_OPENING_BRACE(new AstNode("OPENING_BRACE"), loc);
"}"       return yy::parser::make_CLOSING_BRACE(new AstNode("CLOSING_BRACE"), loc);
"["       return yy::parser::make_OPENING_BRACKET(new AstNode("OPENING_BRACKET"), loc);
"]"       return yy::parser::make_CLOSING_BRACKET(new AstNode("CLOSING_BRACKET"), loc);
"("       return yy::parser::make_OPENING_PAREN(new AstNode("OPENING_PAREN"), loc);
")"       return yy::parser::make_CLOSING_PAREN(new AstNode("CLOSING_PAREN"), loc);
";"       return yy::parser::make_SEMI_COLON(new AstNode("SEMI_COLON"), loc);
"."     return yy::parser::make_DOT(new AstNode("DOT"), loc);
"="     return yy::parser::make_ASSIGNMENT(new AstNode("ASSIGNMENT"), loc);

%{ // Types %}
int     return yy::parser::make_INT(new AstNode("INT"), loc);
boolean    return yy::parser::make_BOOLEAN(new AstNode("BOOLEAN"), loc);
char    return yy::parser::make_CHAR(new AstNode("CHAR"), loc); 
byte    return yy::parser::make_BYTE(new AstNode("BYTE"), loc);
short   return yy::parser::make_SHORT(new AstNode("SHORT"), loc);
"int\[\]"   return yy::parser::make_ARRAY(new AstNode("ARRAY"), loc);

%{ // Literals %}
true                return yy::parser::make_TRUE(new AstNode("TRUE"), loc);
false               return yy::parser::make_FALSE(new AstNode("FALSE"), loc);
\"({Ascii}|{Whitespace}|[\n\"\'\\\0])*\"     return yy::parser::make_STRING_LITERAL(new AstNode("STRING_LITERAL"), loc);
{Integer}           return yy::parser::make_INTEGER(new AstNode("INTEGER"), loc);
null                return yy::parser::make_NULL_TOKEN(new AstNode("NULL_TOKEN"), loc);
\'{Ascii}\'         return yy::parser::make_CHAR_LITERAL(new AstNode("CHAR_LITERAL"), loc);

%{ // Comments %}
"//".*     { }
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/]      { }
\/\*\*.*\*\/    { } 

%{ // Operators %}
"!"     return yy::parser::make_NEGATE(new AstNode("NEGATE"), loc);
"+"     return yy::parser::make_PLUS(new AstNode("PLUS"), loc);
"-"     return yy::parser::make_MINUS(new AstNode("MINUS"), loc);
"*"     return yy::parser::make_ASTERISK(new AstNode("ASTERISK"), loc);
"/"     return yy::parser::make_DIVIDE(new AstNode("DIVIDE"), loc);
"%"     return yy::parser::make_MODULO(new AstNode("MODULO"), loc);
"<"     return yy::parser::make_LESS_THAN(new AstNode("LESS_THAN"), loc);
">"     return yy::parser::make_GREATER_THAN(new AstNode("GREATER_THAN"), loc);
"<="    return yy::parser::make_LESS_THAN_EQUAL(new AstNode("LESS_THAN_EQUAL"), loc);
">="    return yy::parser::make_GREATER_THAN_EQUAL(new AstNode("GREATER_THAN_EQUAL"), loc);
"=="    return yy::parser::make_BOOLEAN_EQUAL(new AstNode("BOOLEAN_EQUAL"), loc);
"!="    return yy::parser::make_NOT_EQUAL(new AstNode("NOT_EQUAL"), loc);
"&"     return yy::parser::make_AMPERSAND(new AstNode("AMPERSAND"), loc);
"|"     return yy::parser::make_PIPE(new AstNode("PIPE"), loc);

{Whitespace}+      loc.step ();
{Identifier}       return yy::parser::make_IDENTIFIER(new AstNode("IDENTIFIER", yytext), loc);

.          {
             throw yy::parser::syntax_error
               (loc, "invalid character: " + std::string(yytext));
}
<<EOF>>    return yy::parser::make_EOF (loc);
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

