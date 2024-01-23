// descriptions
Whitespace = [ \t\f\r\n]
Letter = [a-zA-Z]
Digit = [0-9]
Identifier = {Letter}({Digit}|{Letter}|_)*
Integer = "0" | "-"?[1-9]{Digit}*
Float = {Digit}+ "."  {Digit}+
%%
// rules
%%
// user code