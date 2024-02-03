#ifndef DRIVER_HH
# define DRIVER_HH
# include <string>
# include "parser.hh"
# include <memory>

// Define lexer prototype
# define YY_DECL \
  yy::parser::symbol_type yylex(Driver& drv)
YY_DECL;

// Class to drive parsing/u3/a23dhingra/cs444/cs444-compiler/src/parsing/bison and scanning
class Driver {
public:
  Driver();

  // Return code from last parse; 0 if success and non-zero otherwise
  int result;
  // Parse tree from last parse
  AstNode *root;

  // Run the parser on file F. Return 0 on success
  int parse(const std::string& f);
  // The name of the file being parsed
  std::string file;
  // Whether to generate parser debug traces
  bool trace_parsing;
  // Whether to generate scanner debug traces
  bool trace_scanning;

  // Handling the scanner
  void scan_begin();
  void scan_end();

  // The token's location used by the scanner
  yy::location location;
};
#endif // ! DRIVER_HH
