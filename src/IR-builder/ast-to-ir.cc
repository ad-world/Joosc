#include "ast-to-ir.h"
#include "IR/bin-op/bin-op.h"
#include "IR/comp-unit/comp-unit.h"
#include "IR/func-decl/func-decl.h"
#include "IR/ir_variant.h"
#include "utillities/overload.h"
#include "variant-ast/expressions.h"
#include <memory>
#include <utility>

/***************************************************************
                            EXPRESSIONS
 ***************************************************************/

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(Expression &expr) {
    auto expr_ir = std::visit(util::overload{
        [&](auto &node) { return convert(node); }
    }, expr);
    return expr_ir;
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(Assignment &expr) {
    assert(expr.assigned_to.get());
    assert(expr.assigned_from.get());

    auto dest = convert(*expr.assigned_to);
    auto dest_address = convert(*expr.assigned_to);
    auto src = convert(*expr.assigned_from);

    // TODO: see if this can be simplified (instead of getting dest twice, use temp?)
    auto statement_ir = std::make_unique<StatementIR>(std::in_place_type<MoveIR>, std::move(dest), std::move(src));
    auto expression_ir = std::make_unique<ExpressionIR>(std::in_place_type<MemIR>, std::move(dest_address));
    auto eseq = std::make_unique<ExpressionIR>(std::in_place_type<ESeqIR>, std::move(statement_ir), std::move(expression_ir));

    return eseq;
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(InfixExpression &expr) {
    auto left = convert(*expr.expression1);
    auto right = convert(*expr.expression2);
    BinOpIR::OpType bin_op;

    switch ( expr.op ) {
        case InfixOperator::BOOLEAN_OR:
        case InfixOperator::EAGER_OR:
            bin_op = BinOpIR::OR;
            break;
        case InfixOperator::BOOLEAN_AND:
        case InfixOperator::EAGER_AND:
            bin_op = BinOpIR::AND;
            break;
        case InfixOperator::BOOLEAN_EQUAL:
            bin_op = BinOpIR::EQ;
            break;
        case InfixOperator::BOOLEAN_NOT_EQUAL:
            bin_op = BinOpIR::NEQ;
            break;
        case InfixOperator::PLUS:
            bin_op = BinOpIR::ADD;
            break;
        case InfixOperator::MINUS:
            bin_op = BinOpIR::SUB;
            break;
        case InfixOperator::DIVIDE:
            bin_op = BinOpIR::DIV;
            break;
        case InfixOperator::MULTIPLY:
            bin_op = BinOpIR::MUL;
            break;
        case InfixOperator::MODULO:
            bin_op = BinOpIR::MOD;
            break;
        case InfixOperator::LESS_THAN:
            bin_op = BinOpIR::LT;
            break;
        case InfixOperator::GREATER_THAN:
            bin_op = BinOpIR::GT;
            break;
        case InfixOperator::LESS_THAN_EQUAL:
            bin_op = BinOpIR::LEQ;
            break;
        case InfixOperator::GREATER_THAN_EQUAL:
            bin_op = BinOpIR::GEQ;
            break;
    }

    auto expression_ir = std::make_unique<ExpressionIR>(std::in_place_type<BinOpIR>, bin_op, std::move(left), std::move(right));
    return expression_ir;
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(PrefixExpression &expr) {
    #warning TODO: implement
    assert(expr.expression.get());

    auto expression = convert(*expr.expression);
    switch ( expr.op ) {
        case PrefixOperator::MINUS: {
            auto zero = ConstIR::makeZero();
            auto expression_ir = std::make_unique<ExpressionIR>(
                std::in_place_type<BinOpIR>, BinOpIR::SUB, std::move(zero), std::move(expression)
            );
            return expression_ir;
        }
        case PrefixOperator::NEGATE: {
            // TODO: This can be handled with
        }
    }
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(CastExpression &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(Literal &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ClassInstanceCreationExpression &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(FieldAccess &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(MethodInvocation &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ArrayAccess &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(QualifiedThis &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ArrayCreationExpression &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(QualifiedIdentifier &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(InstanceOfExpression &expr) {
    #warning TODO: implement
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ParenthesizedExpression &expr) {
    #warning TODO: implement
}

/***************************************************************
                            STATEMENTS
 ***************************************************************/

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(Statement &stmt) {
    return std::visit(util::overload{
        [&](auto &node) { return convert(node); }
    }, stmt);
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(IfThenStatement &stmt) {
    #warning TODO: implement
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(IfThenElseStatement &stmt) {
    #warning TODO: implement
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(WhileStatement &stmt) {
    #warning TODO: implement
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ForStatement &stmt) {
    #warning TODO: implement
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(Block &stmt) {
    #warning TODO: implement
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(EmptyStatement &stmt) {
    #warning TODO: implement
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ExpressionStatement &stmt) {
    #warning TODO: implement
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ReturnStatement &stmt) {
    #warning TODO: implement
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(LocalVariableDeclaration &stmt) {
    #warning TODO: implement
}

/***************************************************************
                            OPERATORS
 ***************************************************************/

void IRBuilderVisitor::operator()(ClassDeclaration &node) {
    // TODO: implement
    
    // CREATE CompUnit
    comp_unit = std::make_unique<CompUnitIR>(node.environment->identifier);
}

void IRBuilderVisitor::operator()(MethodDeclaration &node) {
    // TODO: implement

    // Convert CFG to IR
    if ( node.cfg_start == nullptr ) {
        // CFG does not exist
        // => Method is abstract (ie. no body)
        // => Skip creating IR for method

        assert(node.body.get() == nullptr);
        return;
    }

    // Convert body to a vector of StatementIR's
    std::vector<std::unique_ptr<StatementIR>> statements;
    for ( auto &stmt : node.body->statements ) {
        std::unique_ptr<StatementIR> statementIR = convert(stmt);
        if ( statementIR.get() != nullptr ) {
            statements.emplace_back(std::move(statementIR));
        }
    }

    // CREATE FuncDecl
    // FuncDeclIR func_decl = {
    //     node.environment->identifier,
    //     nullptr,                        // TODO: add body
    //     (int) node.parameters.size()
    // };
}

// Rest of the operators are probably not needed?
