#include "variant-ast/ast.h"
#include "variant-ast/visitor.h"
#include <memory>
#include <gtest/gtest.h>

std::unique_ptr<Variant::AstNode> make(Variant::AstNode &&node) {
  return std::make_unique<Variant::AstNode>(std::move(node));
}

TEST(VariantAST, print) {
    // Main code: we would add constructors for a bit less pointer wrangling

    auto exp_stmt = Variant::ExpressionStatement();
    exp_stmt.statement_expression = make(Variant::Integer(5));
    auto then_stmt = std::make_unique<Variant::AstNode>(std::move(exp_stmt));

    auto if_stmt = Variant::IfStatement();
    if_stmt.if_clause = make(Variant::Integer(3));
    if_stmt.then_clause = std::move(then_stmt);

    auto program = Variant::CompilationUnit();
    program.program_statements.push_back(std::move(if_stmt));
    Variant::AstNode ast = std::move(program);

    auto visitor = Variant::PrintVisitor();
    visitor.visit(ast);
}
