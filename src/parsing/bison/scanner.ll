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
Integer         0|-?[1-9]{Digit}*
Float           {Digit}+"."{Digit}+

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

%{ // Keywords %}
if      return yy::parser::make_IF(loc);
while     return yy::parser::make_WHILE(loc);
for     return yy::parser::make_FOR(loc);
else    return yy::parser::make_ELSE(loc);
extends     return yy::parser::make_EXTENDS(loc);
new     return yy::parser::make_NEW(loc);
public    return yy::parser::make_PUBLIC(loc);
implements    return yy::parser::make_IMPLEMENTS(loc);
protected     return yy::parser::make_PROTECTED(loc);
private     return yy::parser::make_PRIVATE(loc);
static    return yy::parser::make_STATIC(loc);
abstract      return yy::parser::make_ABSTRACT(loc);
this      return yy::parser::make_THIS(loc);
void      return yy::parser::make_VOID(loc);
final     return yy::parser::make_FINAL(loc);
import      return yy::parser::make_IMPORT(loc);
class      return yy::parser::make_CLASS(loc);
package     return yy::parser::make_PACKAGE(loc);
interface     return yy::parser::make_INTERFACE(loc);
native    return yy::parser::make_NATIVE(loc);
return    return yy::parser::make_RETURN(loc);

%{ // Brackets / Parenthesis %}
"{"       return yy::parser::make_OPENING_BRACE(loc);
"}"       return yy::parser::make_CLOSING_BRACE(loc);
"["       return yy::parser::make_OPENING_BRACKET(loc);
"]"       return yy::parser::make_CLOSING_BRACKET(loc);
"("       return yy::parser::make_OPENING_PAREN(loc);
")"       return yy::parser::make_CLOSING_PAREN(loc);

%{ // Types %}
int     return yy::parser::make_INT(loc);
boolean    return yy::parser::make_BOOLEAN(loc);
char    return yy::parser::make_CHAR(loc); 
byte    return yy::parser::make_BYTE(loc);
short   return yy::parser::make_SHORT(loc);
"int\[\]"   return yy::parser::make_ARRAY(loc);

%{ // Literals %}
true    return yy::parser::make_TRUE(loc);
false   return yy::parser::make_FALSE(loc);
\"({Letter}|{Whitespace}|{Digit}|[\n\"\'\\\0])*\"     return yy::parser::make_STRING_LITERAL(loc);
{Integer}           return yy::parser::make_INTEGER(loc);
null                return yy::parser::make_NULL_TOKEN(loc);
"\'{Letter}\'"      return yy::parser::make_CHAR_LITERAL(loc);

%{ // Comments %}
"/\*\*.*\*/"    return yy::parser::make_JAVADOC_COMMENT(loc);
"//.*$"         return yy::parser::make_SINGLE_LINE_COMMENT(loc);
"/\*.*\*/"      return yy::parser::make_MULTI_LINE_COMMENT(loc);

%{ // Operators %}
"!"     return yy::parser::make_NEGATE(loc);
"+"     return yy::parser::make_PLUS(loc);
"-"     return yy::parser::make_MINUS(loc);
"*"     return yy::parser::make_ASTERISK(loc);
"/"     return yy::parser::make_DIVIDE(loc);
"%"     return yy::parser::make_MODULO(loc);
"<"     return yy::parser::make_LESS_THAN(loc);
">"     return yy::parser::make_GREATER_THAN(loc);
"<="    return yy::parser::make_LESS_THAN_EQUAL(loc);
">="    return yy::parser::make_GREATER_THAN_EQUAL(loc);
"=="    return yy::parser::make_BOOLEAN_EQUAL(loc);
"!="    return yy::parser::make_NOT_EQUAL(loc);
"&"     return yy::parser::make_AMPERSAND(loc);
"|"     return yy::parser::make_PIPE(loc);
";"     return yy::parser::make_SEMI_COLON(loc);
"."     return yy::parser::make_DOT(loc);
"="     return yy::parser::make_ASSIGNMENT(loc);

{Whitespace}+   loc.step ();
{Identifier}       return yy::parser::make_IDENTIFIER (yytext, loc);
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
