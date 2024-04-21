#include "environment-builder/symboltableentry.h"
#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include <cassert>
#include <set>
#include <unordered_map>
#include <variant>

class DVBuilder : DefaultSkipVisitor<void>{
    // Graph
    static inline struct Graph {
        // Neighbours
        size_t min_colours;
        std::set<MethodDeclarationObject*> methods;
        std::unordered_map<MethodDeclarationObject*, std::set<MethodDeclarationObject*>> neighbours;
        std::unordered_map<MethodDeclarationObject*, int> colour;
    } graph;

    void addMethodsToGraph(std::set<MethodDeclarationObject*> &method_list);
public:
    using DefaultSkipVisitor<void>::operator();

    // Colouring functions
    static void assignColours();
    static void resetColours();
    static void assertColoured();

    // Method assignment getter (returns colour)
    int getAssignment(MethodDeclarationObject* method) {
        assert(graph.methods.find(method) != graph.methods.end()); // Assert method exists in graph
        int colour = graph.colour[method];
        assert(colour > 0); // Assert assigned colour
        return colour;
    }

    void operator()(ClassDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;

    void visit(AstNodeVariant &ast) override {
        std::visit(*this, ast);
    }

};
