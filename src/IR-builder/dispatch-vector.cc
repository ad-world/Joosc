#include "dispatch-vector.h"
#include "environment-builder/symboltableentry.h"
#include "variant-ast/classes.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <set>
#include <iostream>

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

std::set<MethodDeclarationObject*> DVBuilder::removeMethodFromGraph(MethodDeclarationObject* method) {
    std::set<MethodDeclarationObject*> neighbour_list;
    
    graph.methods.erase(method);
    for (auto &func: graph.methods) {
        if (graph.neighbours[func].find(method) != graph.neighbours[func].end()) {
            graph.neighbours[func].erase(method);
            neighbour_list.insert(func);
        }
    }

    return neighbour_list;
}

void DVBuilder::addMethodToGraph(MethodDeclarationObject* method, std::set<MethodDeclarationObject*> neighbour_list) {
    graph.methods.insert(method);

    for(auto &neighbour: neighbour_list) {
        graph.neighbours[neighbour].insert(method);
    }
}


void DVBuilder::assignColours() {
    std::vector<MethodDeclarationObject*> sorted_methods(graph.methods.begin(), graph.methods.end());
    std::sort(sorted_methods.begin(), sorted_methods.end(), [&](MethodDeclarationObject* a, MethodDeclarationObject* b) {
        return graph.neighbours[a].size() > graph.neighbours[b].size();
    });

    for (auto method : sorted_methods) {
        graph.colour[method] = 0;
    }

    std::cout << "Assigning " << sorted_methods.size() << " method" << std::endl;
    for (auto method: sorted_methods) {
        std::cout << "Method name: " << method->containing_type->full_qualified_name << " " <<  method->identifier << std::endl;
        std::cout << "Method has " << graph.neighbours[method].size() << " neighbours." << std::endl;
        int K = static_cast<int>(graph.min_colours);
        std::cout << "K: " << K << std::endl;
        if ( graph.neighbours[method].size() < K ) {
            std::cout << "Method has less than K neighbours" << std::endl;
            auto neighbour_list = DVBuilder::removeMethodFromGraph(method);
            DVBuilder::assignColours();
            DVBuilder::addMethodToGraph(method, neighbour_list);

            int colour = 1;
            bool found_available_colour = false;

            std::cout << "Entering while loop" << std::endl;
            while (!found_available_colour) {
                // Check if the current colour conflicts with any of the method's neighbors
                bool conflict = false;
                for (auto neighbour : graph.neighbours[method]) {
                    if (graph.colour[neighbour] == colour) {
                        conflict = true;
                        break;
                    }
                }
                if (!conflict) {
                    // We found an available colour
                    break;
                }
                colour++;
                if (colour > static_cast<int>(graph.min_colours)) {
                    // We've tried all available colours and couldn't find one
                    graph.colour[method] = 0;
                    return;
                }
            }
            std::cout << "Assigning " << colour << " to " << method->identifier << std::endl;

            graph.colour[method] = colour;
            graph.min_colours = std::max(graph.min_colours, static_cast<size_t>(colour));   
        } else {
            std::cout << "Method has more than K neighbours" << std::endl;

            auto neighbour_list = DVBuilder::removeMethodFromGraph(method);
            DVBuilder::assignColours();
            DVBuilder::addMethodToGraph(method, neighbour_list);

            std::set<int> used_colours;
            for (auto neighbour: graph.neighbours[method]) used_colours.insert(graph.colour[neighbour]);

            if (used_colours.size() < K) {
                int colour = 1;
                while  ( used_colours.find(colour) != used_colours.end()) {
                    colour++;
                }

                graph.colour[method] = colour;
                graph.min_colours = std::max(graph.min_colours, static_cast<size_t>(colour));
            } else {
                graph.colour[method] = 0;
            }
        }
    }

}

// void DVBuilder::assignColours() {
//     // Basic coloring (wastes space)
//     if ( graph.methods.empty() ) { return; }

//     std::set<MethodDeclarationObject*> visited;
//     std::queue<MethodDeclarationObject*> to_visit;
//     to_visit.push(*graph.methods.begin());

//     while ( !to_visit.empty() ) {
//         auto node = to_visit.front();
//         to_visit.pop();

//         int max = 0;
//         for ( auto neighbour : graph.neighbours[node] ) {
//             if ( visited.find(neighbour) != visited.end() ) {
//                 // Neighbour already coloured
//                 max = std::max(max, graph.colour[neighbour]);
//             } else {
//                 to_visit.push(neighbour);
//             }
//         }

//         graph.colour[node] = max + 1;
//         visited.insert(node);
//     }
// }

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
