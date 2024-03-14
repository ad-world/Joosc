#include "parsing/bison/location.hh"
#include "variant-ast/astnode.h"
#include "variant-ast/packages.h"

class AddLocation {
    yy::location loc;
public:
    void operator()(CompilationUnit &node);

    void operator()(QualifiedIdentifier &node);
    void operator()(Identifier &node);

    void operator()(Type &node);
    void operator()(NonArrayType &node);
    void operator()(PrimitiveType &node);

    void operator()(ClassDeclaration &node);
    void operator()(InterfaceDeclaration &node);
    void operator()(FieldDeclaration &node);
    void operator()(MethodDeclaration &node);
    void operator()(VariableDeclarator &node);
    void operator()(FormalParameter &node);
    void operator()(Modifier &node);

    void operator()(LocalVariableDeclaration &node);
    void operator()(Block &node);
    void operator()(IfThenStatement &node);
    void operator()(IfThenElseStatement &node);
    void operator()(WhileStatement &node);
    void operator()(ForStatement &node);
    void operator()(ReturnStatement &node);
    void operator()(EmptyStatement &node);

    void operator()(InfixExpression &node);
    void operator()(PrefixExpression &node);
    void operator()(CastExpression &node);
    void operator()(Assignment &node);
    void operator()(QualifiedThis &node);
    void operator()(ArrayCreationExpression &node);
    void operator()(Literal &node);
    void operator()(ClassInstanceCreationExpression &node);
    void operator()(FieldAccess &node);
    void operator()(ArrayAccess &node);
    void operator()(MethodInvocation &node);
    void operator()(InstanceOfExpression &node);
    void operator()(ParenthesizedExpression &node);

    // Missing before...
    void operator()(Statement &node);
    void operator()(ExpressionStatement &node);
    void operator()(Expression &node);

    void operator()(AstNodeVariant &node);

    static std::string getString(yy::location &loc);

    AddLocation(yy::location &location) {
        if ( location.begin.filename ) {
            const std::string *filename = new std::string(*location.begin.filename);
            yy::position begin{filename, location.begin.line, location.begin.column};
            yy::position end{filename, location.end.line, location.end.column};
            loc = {begin, end};
        } else {
            loc = location;
        }
    }
};
