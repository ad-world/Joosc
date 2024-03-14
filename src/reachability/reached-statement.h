#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "variant-ast/classes.h"

class StatementVisitor : public DefaultSkipVisitor<void> {
public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(MethodDeclaration &node) override;

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }
};
