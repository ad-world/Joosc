#include "driver.h"
#include "parser.hh"
#include <deque>
#include <fstream>
#include <stdexcept>

#define GET_NAME(symbol) \
    (((symbol) == yy::parser::symbol_kind::S_YYEMPTY) ? "YYEMPTY" \
        : yy::parser::symbol_name( \
            static_cast<yy::parser::symbol_kind_type>((symbol))))

Driver::Driver()
: trace_parsing (false), trace_scanning (false) {}

int Driver::parse(const std::string &f) {
    file = f;
    location.initialize(&file);

    scan_begin();
    yy::parser parser(*this, &root);
    parser.set_debug_level(trace_parsing);
    result = parser.parse();
    scan_end();

    // Exception on parser failure
    if ( result != 0 ) { throw std::runtime_error("Parser error."); }

    return result;
}
