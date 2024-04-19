#include "dispatch-vector.h"
#include <algorithm>

void DVBuilder::operator()(ClassDeclaration &node) {
    // Sets the minimum colours to the size of each complete graph
    graph.min_colours = std::min(graph.min_colours, node.environment->method_list.size());

    for ( auto method : node.environment->method_list ) {
        // Add to overall method list
        graph.methods.insert(method);

        // Add all other methods as neighbours
        for ( auto other : node.environment->method_list ) {
            if ( method == other ) { continue; }
            graph.neighbours[method].insert(other);
        }
    }
}

void DVBuilder::operator()(InterfaceDeclaration &node) {
}
