#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include <unordered_map>

class GraphVisitor : public DefaultSkipVisitor<std::string> {
    std::unordered_map<AstNodeVariant*, std::vector<AstNodeVariant*>> map;
public:
    using DefaultSkipVisitor<std::string>::operator();
    void operator()(CompilationUnit &node) override;

    void operator()(QualifiedIdentifier &node) override;
    void operator()(Identifier &node) override;

    void operator()(Type &node) override;
    void operator()(NonArrayType &node) override;
    void operator()(PrimitiveType &node) override;

    void operator()(ClassDeclaration &node) override;
    void operator()(InterfaceDeclaration &node) override;
    void operator()(FieldDeclaration &node) override;
    void operator()(MethodDeclaration &node) override;
    void operator()(VariableDeclarator &node) override;
    void operator()(FormalParameter &node) override;
    void operator()(Modifier &node) override;

    void operator()(LocalVariableDeclaration &node) override;
    void operator()(Block &node) override;
    void operator()(IfThenStatement &node) override;
    void operator()(IfThenElseStatement &node) override;
    void operator()(WhileStatement &node) override;
    void operator()(ForStatement &node) override;
    void operator()(ReturnStatement &node) override;
    void operator()(EmptyStatement &node) override;

    void operator()(InfixExpression &node) override;
    void operator()(PrefixExpression &node) override;
    void operator()(CastExpression &node) override;
    void operator()(Assignment &node) override;
    void operator()(QualifiedThis &node) override;
    void operator()(ArrayCreationExpression &node) override;
    void operator()(Literal &node) override;
    void operator()(ClassInstanceCreationExpression &node) override;
    void operator()(FieldAccess &node) override;
    void operator()(ArrayAccess &node) override;
    void operator()(MethodInvocation &node) override;
    void operator()(InstanceOfExpression &node) override;
    void operator()(ParenthesizedExpression &node) override;
};
