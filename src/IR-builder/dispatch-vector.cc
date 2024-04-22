#include "dispatch-vector.h"
#include "environment-builder/symboltableentry.h"
#include "variant-ast/classes.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <set>

void DVBuilder::addMethodsToGraph(std::set<MethodDeclarationObject*> &method_list) {
    // Sets the minimum colours to the size of each complete graph
    graph.min_colours = std::max(graph.min_colours, method_list.size());

    for ( auto method : method_list ) {
        if ( method->is_constructor ) { continue; }

        // Add to overall method list
        graph.methods.insert(method);

        // Add all other methods as neighbours
        for ( auto other : method_list ) {
            if ( method->is_constructor ) { continue; }

            if ( method == other ) { continue; }
            graph.neighbours[method].insert(other);
            graph.neighbours[other].insert(method);
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

DV::DV(ClassDeclarationObject* class_obj) {
    auto parent_class = class_obj->extended;
    if ( parent_class ) {
        // parent classes
        auto parent_dv = DVBuilder::getDV(class_obj->extended);
        field_vector = parent_dv.field_vector;
    } else {
        // java.lang.Object
    }

    // Add my fields
    for ( auto &field : class_obj->ast_reference->field_declarations ) {
        auto field_obj = field.environment;
        field_vector.push_back(field_obj);
    }

    // Add my methods
    int max_colour = 0;
    for ( auto method : class_obj->method_list ) {
        if ( method->is_constructor ) { continue; }
        int colour = DVBuilder::getAssignment(method);
        max_colour = std::max(colour, max_colour);
    }

    dispatch_vector.resize(max_colour + 1, nullptr);
    for ( auto method : class_obj->method_list ) {
        if ( method->is_constructor ) { continue; }
        int colour = DVBuilder::getAssignment(method);
        dispatch_vector[colour] = method;
    }
}
