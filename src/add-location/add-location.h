#include "parsing/bison/location.hh"
#include "variant-ast/astnode.h"
#include "variant-ast/packages.h"

class AddLocation {
public:
    void operator()(CompilationUnit &node, yy::location &loc);

    void operator()(QualifiedIdentifier &node, yy::location &loc);
    void operator()(Identifier &node, yy::location &loc);

    void operator()(Type &node, yy::location &loc);
    void operator()(NonArrayType &node, yy::location &loc);
    void operator()(PrimitiveType &node, yy::location &loc);

    void operator()(ClassDeclaration &node, yy::location &loc);
    void operator()(InterfaceDeclaration &node, yy::location &loc);
    void operator()(FieldDeclaration &node, yy::location &loc);
    void operator()(MethodDeclaration &node, yy::location &loc);
    void operator()(VariableDeclarator &node, yy::location &loc);
    void operator()(FormalParameter &node, yy::location &loc);
    void operator()(Modifier &node, yy::location &loc);

    void operator()(LocalVariableDeclaration &node, yy::location &loc);
    void operator()(Block &node, yy::location &loc);
    void operator()(IfThenStatement &node, yy::location &loc);
    void operator()(IfThenElseStatement &node, yy::location &loc);
    void operator()(WhileStatement &node, yy::location &loc);
    void operator()(ForStatement &node, yy::location &loc);
    void operator()(ReturnStatement &node, yy::location &loc);
    void operator()(EmptyStatement &node, yy::location &loc);

    void operator()(InfixExpression &node, yy::location &loc);
    void operator()(PrefixExpression &node, yy::location &loc);
    void operator()(CastExpression &node, yy::location &loc);
    void operator()(Assignment &node, yy::location &loc);
    void operator()(QualifiedThis &node, yy::location &loc);
    void operator()(ArrayCreationExpression &node, yy::location &loc);
    void operator()(Literal &node, yy::location &loc);
    void operator()(ClassInstanceCreationExpression &node, yy::location &loc);
    void operator()(FieldAccess &node, yy::location &loc);
    void operator()(ArrayAccess &node, yy::location &loc);
    void operator()(MethodInvocation &node, yy::location &loc);
    void operator()(InstanceOfExpression &node, yy::location &loc);
    void operator()(ParenthesizedExpression &node, yy::location &loc);

    // Missing before...
    void operator()(Statement &node, yy::location &loc);
    void operator()(ExpressionStatement &node, yy::location &loc);
    void operator()(Expression &node, yy::location &loc);

    void operator()(AstNodeVariant &node, yy::location &loc);
};
