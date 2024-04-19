#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include <set>
#include <unordered_map>

class DVBuilder : DefaultSkipVisitor<void>{
    // Graph
    struct Graph {
        // Neighbours
        size_t min_colours = 0;
        std::set<MethodDeclarationObject*> methods;
        std::unordered_map<MethodDeclarationObject*, std::set<MethodDeclarationObject*>> neighbours;
        std::unordered_map<MethodDeclarationObject*, int> colour;
    } graph;

    void addMethodsToGraph(std::set<MethodDeclarationObject*> &method_list);
public:
    void colourMethods();
    void resetColours();
    void assertColoured();

    void operator()(ClassDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;
};
