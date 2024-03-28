%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <string>
# include <sstream>
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

%option noyywrap nounput batch noinput stack
/* %option debug */

Whitespace      [ \b\t\f\r]
Letter          [a-zA-Z]
Digit           [0-9]
Identifier      {Letter}({Digit}|{Letter}|_)*
Integer         0|[1-9]{Digit}*
Float           {Digit}+"."{Digit}+
Ascii           [ -!]|[#-&]|[(-~]
OctalEscapeString   \\[0-7]{1,3}
OctalEscapeChar     \\([0-3][0-7]{0,2}|[0-7][0-7]?)
Escape          \\[tbnrf\\]
InvalidEscape \\[^0-7tbnrf\"'\\]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

%x IN_COMMENT
%x IN_STRING
%x IN_CHAR
%x IN_ESCAPE

%%

%{
    std::stringstream string_buf;
%}

%{
  // A handy shortcut to the location held by the driver.
  yy::location& loc = drv.location;
  // Code run each time yylex is called.
  loc.step ();
%}
private|goto|volatile|super|long|float|double|throws|throw|try|catch|finally|do|switch|break|continue|synchronized|"--"  {
             throw yy::parser::syntax_error
               (loc, "invalid token: '" + std::string(yytext) + "' is not a valid joosc token, only a Java token");
}

if    return yy::parser::make_IF(loc);
while     return yy::parser::make_WHILE(loc);
for     return yy::parser::make_FOR(loc);
else    return yy::parser::make_ELSE(loc);
extends     return yy::parser::make_EXTENDS(loc);
new     return yy::parser::make_NEW(loc);

public    return yy::parser::make_PUBLIC(Modifier::PUBLIC, loc);
protected     return yy::parser::make_PROTECTED(Modifier::PROTECTED, loc);
static    return yy::parser::make_STATIC(Modifier::STATIC, loc);
abstract      return yy::parser::make_ABSTRACT(Modifier::ABSTRACT, loc);
native    return yy::parser::make_NATIVE(Modifier::NATIVE, loc);
final     return yy::parser::make_FINAL(Modifier::FINAL, loc);

implements    return yy::parser::make_IMPLEMENTS(loc);
this      return yy::parser::make_THIS(loc);
import      return yy::parser::make_IMPORT(loc);
class      return yy::parser::make_CLASS(loc);
package     return yy::parser::make_PACKAGE(loc);
interface     return yy::parser::make_INTERFACE(loc);
return    return yy::parser::make_RETURN(loc);
"{"       return yy::parser::make_OPENING_BRACE(loc);
"}"       return yy::parser::make_CLOSING_BRACE(loc);
"["       return yy::parser::make_OPENING_BRACKET(loc);
"]"       return yy::parser::make_CLOSING_BRACKET(loc);
"("       return yy::parser::make_OPENING_PAREN(loc);
")"       return yy::parser::make_CLOSING_PAREN(loc);
";"       return yy::parser::make_SEMI_COLON(loc);
"."     return yy::parser::make_DOT(loc);
"="     return yy::parser::make_ASSIGNMENT(loc);
","     return yy::parser::make_COMMA(loc);

%{ // Types %}
int     return yy::parser::make_INT(PrimitiveType::INT, loc);
boolean    return yy::parser::make_BOOLEAN(PrimitiveType::BOOLEAN, loc);
char    return yy::parser::make_CHAR(PrimitiveType::CHAR, loc); 
byte    return yy::parser::make_BYTE(PrimitiveType::BYTE, loc);
short   return yy::parser::make_SHORT(PrimitiveType::SHORT, loc);
void      return yy::parser::make_VOID(PrimitiveType::VOID, loc);

%{ // Literals %}
true                return yy::parser::make_TRUE(true, loc);
false               return yy::parser::make_FALSE(false, loc);
{Integer}           return yy::parser::make_INTEGER(stol(yytext), loc);
null                return yy::parser::make_NULL_TOKEN(nullptr, loc);

\"      BEGIN(IN_STRING); string_buf.clear();
<IN_STRING>{
    \"      { BEGIN(INITIAL); return yy::parser::make_STRING_LITERAL(string_buf.str(), loc); }
    \\      yy_push_state(IN_ESCAPE);
    .       string_buf << yytext;
}

\'      BEGIN(IN_CHAR); string_buf.clear();
<IN_CHAR>{
    \'      {
        BEGIN(INITIAL);
        std::string outstr = string_buf.str();
        if ( outstr.length() != 1 ) {
            throw yy::parser::syntax_error(loc, "Invalid length of char:" + outstr.length());
        } else {
            return yy::parser::make_CHAR_LITERAL(outstr[0], loc);
        }
    }
    \\      yy_push_state(IN_ESCAPE);
    .       string_buf << yytext;
}

<IN_ESCAPE>{
    t               { string_buf << "\t"; yy_pop_state(); }
    b               { string_buf << "\b"; yy_pop_state(); }
    n               { string_buf << "\n"; yy_pop_state(); }
    r               { string_buf << "\r"; yy_pop_state(); }
    f               { string_buf << "\f"; yy_pop_state(); }
    \\              { string_buf << "\\"; yy_pop_state(); }
    \"              { string_buf << "\""; yy_pop_state(); }
    \'              { string_buf << "'" ; yy_pop_state(); }
    [0-7]{1,2}      { string_buf << (char) std::stoi( yytext, 0, 8 ); yy_pop_state(); }
    [0-3][0-7]{2}   { string_buf << (char) std::stoi( yytext, 0, 8 ); yy_pop_state(); }
    .               { throw yy::parser::syntax_error(loc, "invalid escape:\\"+ std::string(yytext)); }
}

%{ // Comments %}
"//".*     { }

<INITIAL>{
    [/][*]        BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
    "*/"        BEGIN(INITIAL);
    [^*\n]+     // eat
    [*]         // eat
    [\n]        loc.lines (yyleng); loc.step ();
}


%{ // Operators %}
"||"    return yy::parser::make_BOOLEAN_OR(InfixOperator::BOOLEAN_OR, loc);
"&&"    return yy::parser::make_BOOLEAN_AND(InfixOperator::BOOLEAN_AND, loc);
"|"     return yy::parser::make_PIPE(InfixOperator::EAGER_OR, loc);
"&"     return yy::parser::make_AMPERSAND(InfixOperator::EAGER_AND, loc);
"=="    return yy::parser::make_BOOLEAN_EQUAL(InfixOperator::BOOLEAN_EQUAL, loc);
"!="    return yy::parser::make_NOT_EQUAL(InfixOperator::BOOLEAN_NOT_EQUAL, loc);
"+"     return yy::parser::make_PLUS(InfixOperator::PLUS, loc);
"-"     return yy::parser::make_MINUS(InfixOperator::MINUS, loc);
"/"     return yy::parser::make_DIVIDE(InfixOperator::DIVIDE, loc);
"*"     return yy::parser::make_ASTERISK(InfixOperator::MULTIPLY, loc);
"<"     return yy::parser::make_LESS_THAN(InfixOperator::LESS_THAN, loc);
">"     return yy::parser::make_GREATER_THAN(InfixOperator::GREATER_THAN, loc);
"<="    return yy::parser::make_LESS_THAN_EQUAL(InfixOperator::LESS_THAN_EQUAL, loc);
">="    return yy::parser::make_GREATER_THAN_EQUAL(InfixOperator::GREATER_THAN_EQUAL, loc);
instanceof      return yy::parser::make_INSTANCEOF(loc);
"%"     return yy::parser::make_MODULO(InfixOperator::MODULO, loc);
"!"     return yy::parser::make_NEGATE(PrefixOperator::NEGATE, loc);

{Whitespace}+      loc.step ();
{Identifier}       return yy::parser::make_IDENTIFIER(yytext, loc);

.          {
             throw yy::parser::syntax_error
               (loc, "invalid character: " + std::string(yytext));
}
<<EOF>>    return yy::parser::make_EOF (loc);
[\n]+      loc.lines (yyleng); loc.step ();
%%

void Driver::scan_begin() {
  yy_flex_debug = trace_scanning;
  if ( strfile == "" ) {
      if (file.empty()|| file == "-")
      yyin = stdin;
      else if (!(yyin = fopen (file.c_str (), "r")))
      {
          std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
          exit (EXIT_FAILURE);
      }
  } else {
      yy_switch_to_buffer( yy_scan_string(strfile.c_str()) );
  }
}

void Driver::scan_end() {
    if ( strfile == "" ) {
        fclose(yyin);
    } else {
        yy_delete_buffer( YY_CURRENT_BUFFER );
    }
}

