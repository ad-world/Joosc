#include "environment-builder/symboltableentry.h"
#include "exceptions/exceptions.h"
#include "utillities/util.h"
#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include <cassert>
#include <set>
#include <unordered_map>
#include <variant>

class DV {
    DV() = default; // deletes default constructor
public:
    std::vector<MethodDeclarationObject*> dispatch_vector;
    std::vector<FieldDeclarationObject*> field_vector;
    DV(ClassDeclarationObject* class_obj);
};

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
    static inline std::unordered_map<ClassDeclarationObject*, std::unique_ptr<DV>> class_dvs;
public:
    using DefaultSkipVisitor<void>::operator();

    // Colouring functions
    static void assignColours();
    static void resetColours();
    static void assertColoured();

    // Method assignment getter (returns colour)
    static int getAssignment(MethodDeclarationObject* method) {
        assert(graph.methods.find(method) != graph.methods.end()); // Assert method exists in graph
        int colour = graph.colour[method];
        assert(colour > 0); // Assert assigned colour
        return colour;
    }

    static DV getDV(ClassDeclarationObject* class_obj) {
        if ( class_dvs.find(class_obj) == class_dvs.end() ) {
            class_dvs.insert({class_obj, std::make_unique<DV>(class_obj)});
        }
        return *class_dvs.at(class_obj);
    }

    void operator()(ClassDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;

    void visit(AstNodeVariant &ast) override {
        std::visit(*this, ast);
    }

};

class PrintDVS : public DefaultSkipVisitor<void> {
public:
    using DefaultSkipVisitor<void>::operator();

    void operator()(ClassDeclaration &node) override {
        auto dv = DVBuilder::getDV(node.environment);
        std::cout << "==== " << node.environment->identifier << " ====" << std::endl;
        int i = 0;
        for ( auto method : dv.dispatch_vector ) {
            if ( method ) {
                std::cout << "\t" << i << ": "<< method << " - " << method->identifier << std::endl;
            } else {
                // std::cout << i << ": nullptr";
            }
            i++;
        }
        std::cout << std::endl;
    }

    void visit(AstNodeVariant &ast) override {
        std::visit(*this, ast);
    }
};
