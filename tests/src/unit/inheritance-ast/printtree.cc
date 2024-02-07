#include "inheritance-ast/ast.h"
#include "inheritance-ast/visitor.h"
#include <memory>
#include <gtest/gtest.h>

TEST(InheritanceAST, print) {
    // Main code: we would add constructors for a bit less pointer wrangling

    auto exp_stmt = new Inheritance::ExpressionStatement();
    exp_stmt->statement_expression = std::move(std::make_unique<Inheritance::Integer>(5));
    auto then_stmt = std::unique_ptr<Inheritance::Statement>(exp_stmt);

    auto if_stmt = new Inheritance::IfStatement();
    if_stmt->if_clause = std::move(std::make_unique<Inheritance::Integer>(3));
    if_stmt->then_clause = std::move(then_stmt);
    auto stmt1 = std::unique_ptr<Inheritance::Statement>(if_stmt);

    auto program = new Inheritance::CompilationUnit();
    program->program_statements.push_back(std::move(stmt1));
    auto ast = std::unique_ptr<Inheritance::AstNode>(program);

    auto visitor = Inheritance::PrintVisitor();
    ast->accept(visitor);
}
