#include "add-location.h"
#include "parsing/bison/location.hh"
#include "variant-ast/astnode.h"
#include <sstream>
#include <variant>

std::vector<const std::string *> AddLocation::filenames;

void AddLocation::operator()(CompilationUnit &node) {
    node.location = loc;
}

void AddLocation::operator()(QualifiedIdentifier &node) {
    node.location = loc;
}
void AddLocation::operator()(Identifier &node) {
    node.location = loc;
}

void AddLocation::operator()(Type &node) {
    node.location = loc;
}
void AddLocation::operator()(NonArrayType &node) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg);
        }, node);
    } catch (...) {}
}
void AddLocation::operator()(PrimitiveType &node) {
    // Not a class
}

void AddLocation::operator()(ClassDeclaration &node) {
    node.location = loc;
}
void AddLocation::operator()(InterfaceDeclaration &node) {
    node.location = loc;
}
void AddLocation::operator()(FieldDeclaration &node) {
    node.location = loc;
}
void AddLocation::operator()(MethodDeclaration &node) {
    node.location = loc;
}
void AddLocation::operator()(VariableDeclarator &node) {
    node.location = loc;
}
void AddLocation::operator()(FormalParameter &node) {
    node.location = loc;
}
void AddLocation::operator()(Modifier &node) {
    // Not a class
}

void AddLocation::operator()(LocalVariableDeclaration &node) {
    node.location = loc;
}
void AddLocation::operator()(Block &node) {
    node.location = loc;
}
void AddLocation::operator()(IfThenStatement &node) {
    node.location = loc;
}
void AddLocation::operator()(IfThenElseStatement &node) {
    node.location = loc;
}
void AddLocation::operator()(WhileStatement &node) {
    node.location = loc;
}
void AddLocation::operator()(ForStatement &node) {
    node.location = loc;
}
void AddLocation::operator()(ReturnStatement &node) {
    node.location = loc;
}
void AddLocation::operator()(EmptyStatement &node) {
    // Not a class
}

void AddLocation::operator()(InfixExpression &node) {
    node.location = loc;
}
void AddLocation::operator()(PrefixExpression &node) {
    node.location = loc;
}
void AddLocation::operator()(CastExpression &node) {
    node.location = loc;
}
void AddLocation::operator()(Assignment &node) {
    node.location = loc;
}
void AddLocation::operator()(QualifiedThis &node) {
    node.location = loc;
}
void AddLocation::operator()(ArrayCreationExpression &node) {
    node.location = loc;
}
void AddLocation::operator()(Literal &node) {
    // Not a class
}
void AddLocation::operator()(ClassInstanceCreationExpression &node) {
    node.location = loc;
}
void AddLocation::operator()(FieldAccess &node) {
    node.location = loc;
}
void AddLocation::operator()(ArrayAccess &node) {
    node.location = loc;
}
void AddLocation::operator()(MethodInvocation &node) {
    node.location = loc;
}
void AddLocation::operator()(InstanceOfExpression &node) {
    node.location = loc;
}
void AddLocation::operator()(ParenthesizedExpression &node) {
    node.location = loc;
}

void AddLocation::operator()(Statement &node) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg);
        }, node);
    } catch (...) {}
}
void AddLocation::operator()(ExpressionStatement &node) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg);
        }, node);
    } catch (...) {}
}
void AddLocation::operator()(Expression &node) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg);
        }, node);
    } catch (...) {}
}
void AddLocation::operator()(AstNodeVariant &node) {
    try {
        std::visit([&](auto &arg) {
            operator()(arg);
        }, node);
    } catch (...) {}
}

std::string AddLocation::getString(yy::location &loc) {
    std::stringstream location_stream;
    location_stream << loc;
    return location_stream.str();
}
