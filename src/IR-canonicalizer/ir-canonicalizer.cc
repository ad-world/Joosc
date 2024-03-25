#include "ir-canonicalizer.h"
#include "utillities/util.h"
#include "exceptions/exceptions.h"

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
        // [&](BinOpIR &node) {},
        // [&](CallIR &node) {},
        [&](ConstIR &node) { return IRCanonicalizer::LoweredExpression(std::move(node)); },
        // [&](ESeqIR &node) {},
        // [&](MemIR &node) {},
        [&](NameIR &node) { return IRCanonicalizer::LoweredExpression(std::move(node)); },
        [&](TempIR &node) { return IRCanonicalizer::LoweredExpression(std::move(node)); },

        // TEMPORARILY HERE WHILE WIP
        [&](auto &node) { return IRCanonicalizer::LoweredExpression(std::move(node)); }
    }, ir);
}

IRCanonicalizer::LoweredStatement IRCanonicalizer::convert(StatementIR &ir) {
    return std::visit(util::overload {
        // [&](CJumpIR &node) {return {};},
        // [&](ExpIR &node) {return {};},
        // [&](JumpIR &node) {return {};},
        [&](LabelIR &node) { return IRCanonicalizer::LoweredStatement(std::move(node)); },
        // [&](MoveIR &node) {return {};},
        // [&](ReturnIR &node) {return {};},
        // [&](SeqIR &node) {return {};},
        [&](CallIR &node) -> IRCanonicalizer::LoweredStatement {
             THROW_CompilerError("Call IR should not be considered a statement before conversion"); 
        },

        // TEMPORARILY HERE WHILE WIP
        [&](auto &node) { return IRCanonicalizer::LoweredStatement(std::move(node)); }
    }, ir);
}
