#include "driver.h"
#include "parser.hh"

Driver::Driver()
  : trace_parsing (false), trace_scanning (false) {}

int Driver::parse(const std::string &f) {
  file = f;
  location.initialize(&file);
  scan_begin();
  yy::parser parser(*this);
  parser.set_debug_level(trace_parsing);
  result = parser.parse();
  scan_end();
  return result;
}
