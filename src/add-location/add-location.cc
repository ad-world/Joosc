#include "add-location.h"
#include "parsing/bison/location.hh"
#include "variant-ast/astnode.h"
#include <sstream>
#include <variant>

void AddLocation::operator()(CompilationUnit &node, yy::location &loc) {
    node.location = loc;
}

void AddLocation::operator()(QualifiedIdentifier &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(Identifier &node, yy::location &loc) {
    node.location = loc;
}

void AddLocation::operator()(Type &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(NonArrayType &node, yy::location &loc) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg, loc);
        }, node);
    } catch (...) {}
}
void AddLocation::operator()(PrimitiveType &node, yy::location &loc) {
    // Not a class
}

void AddLocation::operator()(ClassDeclaration &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(InterfaceDeclaration &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(FieldDeclaration &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(MethodDeclaration &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(VariableDeclarator &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(FormalParameter &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(Modifier &node, yy::location &loc) {
    // Not a class
}

void AddLocation::operator()(LocalVariableDeclaration &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(Block &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(IfThenStatement &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(IfThenElseStatement &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(WhileStatement &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(ForStatement &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(ReturnStatement &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(EmptyStatement &node, yy::location &loc) {
    // Not a class
}

void AddLocation::operator()(InfixExpression &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(PrefixExpression &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(CastExpression &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(Assignment &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(QualifiedThis &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(ArrayCreationExpression &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(Literal &node, yy::location &loc) {
    // Not a class
}
void AddLocation::operator()(ClassInstanceCreationExpression &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(FieldAccess &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(ArrayAccess &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(MethodInvocation &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(InstanceOfExpression &node, yy::location &loc) {
    node.location = loc;
}
void AddLocation::operator()(ParenthesizedExpression &node, yy::location &loc) {
    node.location = loc;
}

void AddLocation::operator()(Statement &node, yy::location &loc) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg, loc);
        }, node);
    } catch (...) {}
}
void AddLocation::operator()(ExpressionStatement &node, yy::location &loc) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg, loc);
        }, node);
    } catch (...) {}
}
void AddLocation::operator()(Expression &node, yy::location &loc) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg, loc);
        }, node);
    } catch (...) {}
}
void AddLocation::operator()(AstNodeVariant &node, yy::location &loc) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg, loc);
        }, node);
    } catch (...) {}
}

std::string AddLocation::getString(yy::location &loc) {
    std::stringstream location_stream;
    location_stream << loc;
    return location_stream.str();
}
