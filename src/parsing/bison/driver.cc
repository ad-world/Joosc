#include "driver.h"
#include "parser.hh"
#include "../../ast/ast.h"
#include <deque>
#include <fstream>
#include <stdexcept>

// #define GRAPHVIZ
#define GET_NAME(symbol) \
    (((symbol) == yy::parser::symbol_kind::S_YYEMPTY) ? "YYEMPTY" \
        : yy::parser::symbol_name( \
            static_cast<yy::parser::symbol_kind_type>((symbol))))

Driver::Driver()
: trace_parsing (false), trace_scanning (false) {}

int Driver::parse(const std::string &f) {
#ifdef GRAPHVIZ
    ofstream graph;
    graph.open("graph.gv");
#endif

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

#ifdef GRAPHVIZ
    graph << "digraph G {\n\tgraph [layout=dot rankdir=LR]";
#endif

    while ( !q.empty() ) {
        AstNode * curr = q.front();
        int curr_depth = depth.front();

#ifdef PRINT_TREE
        for (int j = 0; j < curr_depth; j++) std::cout << '\t';
        std::cout << yy::parser::symbol_name(static_cast<yy::parser::symbol_kind_type>(curr->type)) << std::endl;
#endif 

#ifdef GRAPHVIZ
        graph << "\n\t\"" << curr << "\" [label=\"" << GET_NAME(curr->type);
        if ( curr->value.has_value() ) {
            auto & innerValue = curr->value.value();
            graph << "\\n";
            if ( holds_alternative<string>(innerValue) ) {
                graph << get<string>(innerValue);
            } else if ( holds_alternative<long int>(innerValue) ) {
                graph << get<long int>(innerValue);
            }
        }
        graph << "\"]";
        graph << "\n\t\"" << curr << "\" -> {";
#endif

        q.pop_front();
        depth.pop_front();
        for ( auto child : curr->children ) {
            q.push_back(child);
            depth.push_back(curr_depth + 1);
#ifdef GRAPHVIZ
            graph << "\"" << child << "\" ";
        }
        graph << "}";
    }

    graph << "\n}" << endl;
    graph.close();
#else
        }
    }
#endif

    return result;
}
