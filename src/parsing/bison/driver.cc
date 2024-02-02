#include "driver.h"
#include "parser.hh"
#include "../../ast/ast.h"
#include <deque>
#include <stdexcept>

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

    // Traverse and print parse tree
    deque<AstNode *> q;
    deque<int> depth;
    q.push_back(root);
    depth.push_back(0);

    while ( !q.empty() ) {
        AstNode * curr = q.front();
        int curr_depth = depth.front();

#ifdef PRINT_PARSETREE
        for (int j = 0; j < curr_depth; j++) std::cout << '\t';
        std::cout << yy::parser::symbol_name(static_cast<yy::parser::symbol_kind_type>(curr->type)) << std::endl;
#endif 

        q.pop_front();
        depth.pop_front();
        for ( auto child : curr->children ) {
            q.push_back(child);
            depth.push_back(curr_depth + 1);
        }
    }

    return result;
}
