#include "cfg-builder/cfg-visitor.h"
#include "variant-ast/astnode.h"
#include "variant-ast/classes.h"
#include <fstream>

class CfgGraph : public CfgVisitor {
    std::string graph_file = "graphs/cfggraph.gv";
    std::ofstream graph;
    std::vector<AstNodeVariant>* asts;
public:
    void operator()(CfgStatement *stmt) override;
    void operator()(CfgExpression *expr) override;

    // void operator()(MethodDeclaration &node) override { Graph(node.cfg_start); }
    void operator()(ClassDeclaration &node) override { Graph(node); }

    // Create graph for a single cfg
    // void Graph(CfgNode *node) {
    //     graph.open(graph_file);
    //     graph << "digraph G {\n";
    //     this->visit_child(node);
    //     graph << "}\n";
    //     graph.close();
    // }

    // Create sub-graphs for a class
    void Graph(ClassDeclaration &node) {
        graph << "labelloc=\"t\"\n";
        graph << "label=\"" << node.class_name->name << "\"\n";
        for ( auto &method : node.method_declarations ) {
            if ( method.cfg_start ) {
                graph << "subgraph {\n";
                graph << "cluster=true\n";
                graph << "label=\"" << method.getSignature() << "\"\n";
                visited.clear();
                this->visit_child(method.cfg_start);
                graph << "}\n";
            }
        }
    }

    CfgGraph(std::vector<AstNodeVariant>& asts) : asts{&asts} {}
    ~CfgGraph() {
        graph.open(graph_file);
        for ( auto& ast : *asts ) {
            graph << "digraph G {\n";
            this->visit(ast);
            graph << "}\n";
        }
        graph.close();
    }
};
