#include "dispatch-vector.h"
#include <algorithm>
#include <set>

void DVBuilder::addMethodsToGraph(std::set<MethodDeclarationObject*> &method_list) {
    // Sets the minimum colours to the size of each complete graph
    graph.min_colours = std::max(graph.min_colours, method_list.size());

    for ( auto method : method_list ) {
        // Add to overall method list
        graph.methods.insert(method);

        // Add all other methods as neighbours
        for ( auto other : method_list ) {
            if ( method == other ) { continue; }
            graph.neighbours[method].insert(other);
        }
    }
}

void DVBuilder::operator()(ClassDeclaration &node) {
    addMethodsToGraph(node.environment->method_list);
}

void DVBuilder::operator()(InterfaceDeclaration &node) {
    addMethodsToGraph(node.environment->method_list);
}
