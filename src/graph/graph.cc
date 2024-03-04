#include "graph.h"
#include "variant-ast/astnode.h"

void GraphVisitor::operator()(CompilationUnit &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

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

void GraphVisitor::operator()(QualifiedIdentifier &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    for (auto &child : node.identifiers) {
        this->operator()(child);
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Identifier &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(Type &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    if ( node.non_array_type ) {
        children.push_back((AstNodeVariant*) node.non_array_type.get());
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(NonArrayType &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(PrimitiveType &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(ClassDeclaration &node) {
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
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;

    for (auto &child : node.modifiers) {
        this->operator()(child);
    }
    if (node.type) {
        this->operator()(*node.type);
    }
    if (node.variable_declarator) {
        this->operator()(*node.variable_declarator);
    }

    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(MethodDeclaration &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(VariableDeclarator &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(FormalParameter &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Modifier &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(LocalVariableDeclaration &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Block &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(IfThenStatement &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(IfThenElseStatement &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(WhileStatement &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ForStatement &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ReturnStatement &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(EmptyStatement &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}

void GraphVisitor::operator()(InfixExpression &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(PrefixExpression &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(CastExpression &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Assignment &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(QualifiedThis &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ArrayCreationExpression &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(Literal &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ClassInstanceCreationExpression &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(FieldAccess &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ArrayAccess &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(MethodInvocation &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(InstanceOfExpression &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
void GraphVisitor::operator()(ParenthesizedExpression &node) {
    this->visit_children(node);
    std::vector<AstNodeVariant*> children;
    // TODO: Add children
    map.insert({(AstNodeVariant*) &node, children});
}
