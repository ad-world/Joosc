#include "graph.h"
#include "variant-ast/names.h"
#include "variant-ast/primitivetype.h"
#include <fstream>
#include <queue>
#include <sstream>
#include <iostream>
#include <string>
#include <variant>
#include "add-location/add-location.h"

#define GRAPH_LOCATION

#ifdef GRAPH_LOCATION
#define PRINT_LOC \
    label_map[(AstNodeVariant*)&node] += "\n\n" + AddLocation::getString(node.location)
#endif

void GraphVisitor::operator()(CompilationUnit &node) {
    label_map[(AstNodeVariant*)&node] = "CompilationUnit";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    root = &node;

    if (node.package_declaration) {
        children.push_back((AstNodeVariant*) node.package_declaration.get());
    }
    for (auto &child : node.single_type_import_declaration) {
        children.push_back((AstNodeVariant*) &(child));
    }
    for (auto &child : node.type_import_on_demand_declaration) {
        children.push_back((AstNodeVariant*) &(child));
    }
    for (auto &child : node.class_declarations) {
        children.push_back((AstNodeVariant*) &(child));
    }
    for (auto &child : node.interface_declarations) {
        children.push_back((AstNodeVariant*) &(child));
    }

    map.insert({(AstNodeVariant*) &node, children});
}

std::string classificationToString(Classification classification) {
    switch (classification) {
        case EXPRESSION_NAME: return "EXPRESSION_NAME";
        case TYPE_NAME: return "TYPE_NAME";
        case PACKAGE_NAME: return "PACKAGE_NAME";
        case METHOD_NAME: return "METHOD_NAME";
        case UNCLASSIFIED: return "UNCLASSIFIED";
        default: return "ERROR CLASSIFICATION NOT FOUND";
    }
}

void GraphVisitor::operator()(QualifiedIdentifier &node) {
    label_map[(AstNodeVariant*)&node] = "QualifiedIdentifier\n" + node.getQualifiedName() + "\n" + classificationToString(node.getClassification());
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    for (auto &child : node.identifiers) {
        children.push_back((AstNodeVariant*) &child);
    }

    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(Identifier &node) {
    label_map[(AstNodeVariant*)&node] = "Identifier\n" + node.name + "\n" + classificationToString(node.classification);
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(Type &node) {
    label_map[(AstNodeVariant*)&node] = "Type";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if ( node.non_array_type ) {
        children.push_back((AstNodeVariant*) node.non_array_type.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(NonArrayType &node) {
    label_map[(AstNodeVariant*)&node] = "NonArrayType";
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(PrimitiveType &node) {
    label_map[(AstNodeVariant*)&node] = "PrimitiveType\n" + getPrimitiveName(node);
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(ClassDeclaration &node) {
    label_map[(AstNodeVariant*)&node] = "ClassDeclaration\n" + node.class_name->name;
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    for (auto &child : node.modifiers) {
        children.push_back((AstNodeVariant*) &child);
    }
    if (node.class_name) {
        children.push_back((AstNodeVariant*) node.class_name.get());
    }
    if (node.extends_class) {
        children.push_back((AstNodeVariant*) node.extends_class.get());
    }
    for (auto &child : node.implements) {
        children.push_back((AstNodeVariant*) &child);
    }
    for (auto &child : node.field_declarations) {
        children.push_back((AstNodeVariant*) &child);
    }
    for (auto &child : node.method_declarations) {
        children.push_back((AstNodeVariant*) &child);
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(InterfaceDeclaration &node) {
    label_map[(AstNodeVariant*)&node] = "InterfaceDeclaration\n" + node.interface_name->name;
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    for (auto &child : node.modifiers) {
        children.push_back((AstNodeVariant*) &child);
    }
    if (node.interface_name) {
        children.push_back((AstNodeVariant*) node.interface_name.get());
    }
    for (auto &child : node.extends_class) {
        children.push_back((AstNodeVariant*) &child);
    }
    for (auto &child : node.method_declarations) {
        children.push_back((AstNodeVariant*) &child);
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(FieldDeclaration &node) {
    label_map[(AstNodeVariant*)&node] = "FieldDeclaration\n" + node.variable_declarator->variable_name->name;
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    for (auto &child : node.modifiers) {
        children.push_back((AstNodeVariant*) &child);
    }
    if (node.type) {
        children.push_back((AstNodeVariant*) node.type.get());
    }
    if (node.variable_declarator) {
        children.push_back((AstNodeVariant*) node.variable_declarator.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(MethodDeclaration &node) {
    label_map[(AstNodeVariant*)&node] = "MethodDeclaration\n" + node.function_name->name;
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    for (auto &child : node.modifiers) {
        children.push_back((AstNodeVariant*) &child);
    }
    if (node.type) {
        children.push_back((AstNodeVariant*) node.type.get());
    }
    if (node.function_name) {
        children.push_back((AstNodeVariant*) node.function_name.get());
    }
    for (auto &child : node.parameters) {
        children.push_back((AstNodeVariant*) &child);
    }
    if (node.body) {
        children.push_back((AstNodeVariant*) node.body.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(VariableDeclarator &node) {
    label_map[(AstNodeVariant*)&node] = "VariableDeclarator\n" + node.variable_name->name;
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.variable_name) {
        children.push_back((AstNodeVariant*) node.variable_name.get());
    }
    if (node.expression) {
        children.push_back((AstNodeVariant*) node.expression.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(FormalParameter &node) {
    label_map[(AstNodeVariant*)&node] = "FormalParameter\n" + node.parameter_name->name;
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.type) {
        children.push_back((AstNodeVariant*) node.type.get());
    }
    if (node.parameter_name) {
        children.push_back((AstNodeVariant*) node.parameter_name.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Modifier &node) {
    std::string mod_type;
    switch (node) {
        case Modifier::PUBLIC:
            mod_type = "PUBLIC";
            break;
        case Modifier::PROTECTED:
            mod_type = "PROTECTED";
            break;
        case Modifier::ABSTRACT:
            mod_type = "ABSTRACT";
            break;
        case Modifier::STATIC:
            mod_type = "STATIC";
            break;
        case Modifier::NATIVE:
            mod_type = "NATIVE";
            break;
        case Modifier::FINAL:
            mod_type = "FINAL";
            break;
    }
    label_map[(AstNodeVariant*)&node] = "Modifier\n" + mod_type;
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(LocalVariableDeclaration &node) {
    label_map[(AstNodeVariant*)&node] = "LocalVariableDeclaration\n" + node.variable_declarator->variable_name->name;
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.type) {
        children.push_back((AstNodeVariant*) node.type.get());
    }
    if (node.variable_declarator) {
        children.push_back((AstNodeVariant*) node.variable_declarator.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Block &node) {
    label_map[(AstNodeVariant*)&node] = "Block";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif

    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    for (auto &child : node.statements) {
        children.push_back((AstNodeVariant*) &child);
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(IfThenStatement &node) {
    label_map[(AstNodeVariant*)&node] = "IfThenStatement";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.if_clause) {
        children.push_back((AstNodeVariant*) node.if_clause.get());
    }
    if (node.then_clause) {
        children.push_back((AstNodeVariant*) node.then_clause.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(IfThenElseStatement &node) {
    label_map[(AstNodeVariant*)&node] = "IfThenElseStatement";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.if_clause) {
        children.push_back((AstNodeVariant*) node.if_clause.get());
    }
    if (node.then_clause) {
        children.push_back((AstNodeVariant*) node.then_clause.get());
    }
    if (node.else_clause) {
        children.push_back((AstNodeVariant*) node.else_clause.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(WhileStatement &node) {
    label_map[(AstNodeVariant*)&node] = "WhileStatement";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.condition_expression) {
        children.push_back((AstNodeVariant*) node.condition_expression.get());
    }
    if (node.body_statement) {
        children.push_back((AstNodeVariant*) node.body_statement.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ForStatement &node) {
    label_map[(AstNodeVariant*)&node] = "ForStatement";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.init_statement) {
        children.push_back((AstNodeVariant*) node.init_statement.get());
    }
    if (node.condition_expression) {
        children.push_back((AstNodeVariant*) node.condition_expression.get());
    }
    if (node.update_statement) {
        children.push_back((AstNodeVariant*) node.update_statement.get());
    }
    if (node.body_statement) {
        children.push_back((AstNodeVariant*) node.body_statement.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ReturnStatement &node) {
    label_map[(AstNodeVariant*)&node] = "ReturnStatement";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.return_expression) {
        children.push_back((AstNodeVariant*) node.return_expression.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(EmptyStatement &node) {
    label_map[(AstNodeVariant*)&node] = "EmptyStatement";
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(InfixExpression &node) {
    label_map[(AstNodeVariant*)&node] = "InfixExpression";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.expression1) {
        children.push_back((AstNodeVariant*) node.expression1.get());
    }
    if (node.expression2) {
        children.push_back((AstNodeVariant*) node.expression2.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(PrefixExpression &node) {
    label_map[(AstNodeVariant*)&node] = "PrefixExpression";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.expression) {
        children.push_back((AstNodeVariant*) node.expression.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(CastExpression &node) {
    label_map[(AstNodeVariant*)&node] = "CastExpression";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.type) {
        children.push_back((AstNodeVariant*) node.type.get());
    }
    if (node.expression) {
        children.push_back((AstNodeVariant*) node.expression.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Assignment &node) {
    label_map[(AstNodeVariant*)&node] = "Assignment";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.assigned_to) {
        children.push_back((AstNodeVariant*) node.assigned_to.get());
    }
    if (node.assigned_from) {
        children.push_back((AstNodeVariant*) node.assigned_from.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(QualifiedThis &node) {
    label_map[(AstNodeVariant*)&node] = "QualifiedThis";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.qualified_this) {
        children.push_back((AstNodeVariant*) node.qualified_this.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ArrayCreationExpression &node) {
    label_map[(AstNodeVariant*)&node] = "ArrayCreationExpression";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.type) {
        children.push_back((AstNodeVariant*) node.type.get());
    }
    if (node.expression) {
        children.push_back((AstNodeVariant*) node.expression.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Literal &node) {
    std::string val;
    if ( auto obj = std::get_if<int64_t>(&node) ) {
        val = std::to_string(*obj);
    } else if ( auto obj = std::get_if<bool>(&node) ) {
        val = *obj ? "true" : "false";
    } else if ( auto obj = std::get_if<char>(&node) ) {
        val = *obj;
        val = "'" + val + "'";
    } else if ( auto obj = std::get_if<std::string>(&node) ) {
        val = *obj;
        val = "\\\"" + val + "\\\"";
    } else {
        val = "NULL";
    }

    label_map[(AstNodeVariant*)&node] = "Literal\n" + val;
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ClassInstanceCreationExpression &node) {
    label_map[(AstNodeVariant*)&node] = "ClassInstanceCreationExpression";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.class_name) {
        children.push_back((AstNodeVariant*) node.class_name.get());
    }
    for (auto &child : node.arguments) {
        children.push_back((AstNodeVariant*) &child);
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(FieldAccess &node) {
    label_map[(AstNodeVariant*)&node] = "FieldAccess";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.expression) {
        children.push_back((AstNodeVariant*) node.expression.get());
    }
    if (node.identifier) {
        children.push_back((AstNodeVariant*) node.identifier.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ArrayAccess &node) {
    label_map[(AstNodeVariant*)&node] = "ArrayAccess";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.array) {
        children.push_back((AstNodeVariant*) node.array.get());
    }
    if (node.selector) {
        children.push_back((AstNodeVariant*) node.selector.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(MethodInvocation &node) {
    label_map[(AstNodeVariant*)&node] = "MethodInvocation";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.parent_expr) {
        children.push_back((AstNodeVariant*) node.parent_expr.get());
    }
    if (node.method_name) {
        children.push_back((AstNodeVariant*) node.method_name.get());
    }
    for (auto &child : node.arguments) {
        children.push_back((AstNodeVariant*) &child);
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(InstanceOfExpression &node) {
    label_map[(AstNodeVariant*)&node] = "InstanceOfExpression";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.expression) {
        children.push_back((AstNodeVariant*) node.expression.get());
    }
    if (node.type) {
        children.push_back((AstNodeVariant*) node.type.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ParenthesizedExpression &node) {
    label_map[(AstNodeVariant*)&node] = "ParenthesizedExpression";
#ifdef GRAPH_LOCATION
    PRINT_LOC;
#endif
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if (node.expression) {
        children.push_back((AstNodeVariant*) node.expression.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}

std::string GraphVisitor::getGraph() {
    std::stringstream graph;

    graph << "digraph G {\n\tgraph [layout=dot rankdir=LR]";

    std::queue<AstNodeVariant*> q;
    q.push((AstNodeVariant*) root);

    while ( !q.empty() ) {
        AstNodeVariant* curr = q.front();
        q.pop();

        std::string type;
        try {
            type = label_map.at(curr);
        } catch (...) {
            type = "ERROR getting type";
        }

        graph << "\n\t"
            << "\"" << curr << "\""
            << " [label=\"" << type << "\"]";

        graph << "\n\t"
            << "\"" << curr << "\""
            << " -> {";

        try {
            for ( auto child : map.at(curr) ) {
                q.push(child);
                graph << "\"" << child << "\" ";
            }
        } catch (...) {}

        graph << "}";
    }

    graph << "\n"
        << "}"
        << "\n";

    return graph.str();
}

std::string GraphVisitor::visit(AstNodeVariant &node) {
    std::visit(*this, node);
    return getGraph();
}

GraphVisitor::GraphVisitor(std::vector<AstNodeVariant>& asts) : asts{&asts} {}

GraphVisitor::~GraphVisitor() {
    std::ofstream graph;
    graph.open("graphs/graph.gv");
    for ( auto& ast : *asts ) {
        graph << visit(ast);
    }
    graph.close();
}
