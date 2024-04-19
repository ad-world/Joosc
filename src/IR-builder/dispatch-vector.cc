#include "dispatch-vector.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
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

void DVBuilder::assignColours() {
    // Basic coloring (wastes space)
    if ( graph.methods.empty() ) { return; }

    std::set<MethodDeclarationObject*> visited;
    std::queue<MethodDeclarationObject*> to_visit;
    to_visit.push(*graph.methods.begin());

    while ( !to_visit.empty() ) {
        auto node = to_visit.front();
        to_visit.pop();

        int max = 0;
        for ( auto neighbour : graph.neighbours[node] ) {
            if ( visited.find(neighbour) != visited.end() ) {
                // Neighbour already coloured
                max = std::max(max, graph.colour[neighbour]);
            } else {
                to_visit.push(neighbour);
            }
        }

        graph.colour[node] = max + 1;
        visited.insert(node);
    }
}

void DVBuilder::resetColours() {
    // Quick and easy way to reset colours
    for ( auto node : graph.methods ) {
        graph.colour[node] = 0;
    }
}

void DVBuilder::assertColoured() {
    // Ensure that each method
    // - has colour > 0
    // - does not share a colour with neighbours
    for ( auto node : graph.methods ) {
        // Loop through each method
        int colour = graph.colour[node];
        assert(colour > 0);
        
        for ( auto neighbour : graph.neighbours[node] ) {
            assert(colour != graph.colour[neighbour]);
        }
    }
}

void DVBuilder::operator()(ClassDeclaration &node) {
    addMethodsToGraph(node.environment->method_list);
}

void DVBuilder::operator()(InterfaceDeclaration &node) {
    addMethodsToGraph(node.environment->method_list);
}
