#include "ir-canonicalizer.h"
#include "utillities/util.h"
#include "exceptions/exceptions.h"
#include "concatenate.h"

IRCanonicalizer::LoweredStatement::LoweredStatement(std::vector<StatementIR> statements)
    : statements{std::move(statements)} {}

IRCanonicalizer::LoweredExpression::LoweredExpression(
        std::vector<StatementIR> statements, std::unique_ptr<ExpressionIR> expression
    ) : statements{std::move(statements)}, expression{std::move(expression)} {}

IRCanonicalizer::LoweredExpression::LoweredExpression(
        std::vector<StatementIR> statements, ExpressionIR expression
    ) : statements{std::move(statements)}, expression{std::make_unique<ExpressionIR>(std::move(expression))} {}

void IRCanonicalizer::operator()(IR &ir) { convert(ir); }

IRCanonicalizer::LoweredExpression IRCanonicalizer::operator()(ExpressionIR &ir) { return convert(ir); }
IRCanonicalizer::LoweredStatement IRCanonicalizer::operator()(StatementIR &ir) { return convert(ir); }

void IRCanonicalizer::convert(IR &ir) {
    std::visit(util::overload {
        [&](CompUnitIR &node) {},
        [&](FuncDeclIR &node) {},
        [&](ExpressionIR &node) { convert(node); },
        [&](StatementIR &node) { convert(node); }
    }, ir);
}

IRCanonicalizer::LoweredExpression IRCanonicalizer::convert(ExpressionIR &ir) {
    return std::visit(util::overload {
        [&](BinOpIR &node) {
            // Unoptimized form: assumes right's side effects can affect left
            IRCanonicalizer::LoweredExpression lowered1 = convert(node.getLeft());
            IRCanonicalizer::LoweredExpression lowered2 = convert(node.getRight());
            
            std::string temp_name = TempIR::generateName();

            auto statements = concatenate(
                lowered1.statements,
                MoveIR(std::make_unique<ExpressionIR>(TempIR(temp_name)), std::move(lowered1.expression)),
                lowered2.statements
            );

            auto expression = BinOpIR(node.opType(), std::make_unique<ExpressionIR>(TempIR(temp_name)), std::move(lowered2.expression));

            return IRCanonicalizer::LoweredExpression(std::move(statements), std::move(expression));
        },

        [&](CallIR &node) {
            IRCanonicalizer::LoweredExpression result;

            std::vector<std::unique_ptr<ExpressionIR>> arg_temporaries;

            for (auto& arg : node.getArgs()) {
                auto temporary = TempIR(TempIR::generateName(ARG_TEMPORARY_PREFIX));
                arg_temporaries.push_back(std::make_unique<ExpressionIR>(temporary));

                auto lowered = convert(*arg);

                result.statements = concatenate(
                    result.statements, 
                    lowered.statements, 
                    MoveIR(std::make_unique<ExpressionIR>(temporary), std::move(lowered.expression))
                );
            }

            auto target = std::make_unique<ExpressionIR>(std::move(node.getTarget()));

            result.statements.emplace_back(CallIR(std::move(target), std::move(arg_temporaries)));
            result.expression = std::make_unique<ExpressionIR>(TempIR(RETURN_TEMPORARY_NAME));

            return result;
        },

        [&](ConstIR &node) { 
            // Leave alone
            return IRCanonicalizer::LoweredExpression(std::move(node)); 
        },

        [&](ESeqIR &node) {
            // Concatenate the lowered statements followed by the lowered statements of the expression, then the lowered expression
            IRCanonicalizer::LoweredStatement lowered_statement = convert(node.getStmt());
            IRCanonicalizer::LoweredExpression lowered_expression = convert(node.getExpr());

            auto statements = concatenate(
                lowered_statement.statements,
                lowered_expression.statements
            );

            auto expression = std::move(lowered_expression.expression);

            return IRCanonicalizer::LoweredExpression(std::move(statements), std::move(expression));
        },

        // [&](MemIR &node) {},

        [&](NameIR &node) { 
            // Leave alone
            return IRCanonicalizer::LoweredExpression(std::move(node)); 
        },

        [&](TempIR &node) { 
            // Leave alone
            return IRCanonicalizer::LoweredExpression(std::move(node)); 
        },

        // TEMPORARILY HERE WHILE WIP
        [&](auto &node) { return IRCanonicalizer::LoweredExpression(std::move(node)); }
    }, ir);
}

IRCanonicalizer::LoweredStatement IRCanonicalizer::convert(StatementIR &ir) {
    return std::visit(util::overload {
        // [&](CJumpIR &node) {return {};},

        [&](ExpIR &node) {
            // Throw away the expression
            IRCanonicalizer::LoweredExpression lowered_expression = convert(node.getExpr());
            return IRCanonicalizer::LoweredStatement(std::move(lowered_expression.statements));
        },

        // [&](JumpIR &node) {return {};},

        [&](LabelIR &node) {
            // Leave alone 
            return IRCanonicalizer::LoweredStatement(std::move(node)); 
        },

        // [&](MoveIR &node) {return {};},

        [&](ReturnIR &node) {
            // Put statements in front of return expression after lowering
            IRCanonicalizer::LoweredExpression lowered_expression = convert(node.getRet());
            auto result = IRCanonicalizer::LoweredStatement(std::move(lowered_expression.statements));
            result.statements.emplace_back(ReturnIR(std::move(lowered_expression.expression)));
            return result;
        },

        [&](SeqIR &node) {
            // Lower all contained statements and concatenate them
            IRCanonicalizer::LoweredStatement result;
            for (auto& stmt : node.getStmts()) {
                IRCanonicalizer::LoweredStatement lowered_statement = convert(*stmt);
                for (auto& lowered_stmt : lowered_statement.statements) {
                    result.statements.emplace_back(std::move(lowered_stmt));
                }
            }
            return result;
        },

        [&](CallIR &node) -> IRCanonicalizer::LoweredStatement {
             THROW_CompilerError("Call IR should not be considered a statement before conversion"); 
        },

        // TEMPORARILY HERE WHILE WIP
        [&](auto &node) { return IRCanonicalizer::LoweredStatement(std::move(node)); }
    }, ir);
}
