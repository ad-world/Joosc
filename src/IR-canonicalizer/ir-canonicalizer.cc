#include "ir-canonicalizer.h"
#include "utillities/util.h"
#include "IR/ir.h"
#include "exceptions/exceptions.h"

void IRCanonicalizer::operator()(IR &ir) { convert(ir); }
void IRCanonicalizer::operator()(ExpressionIR &ir) { convert(ir); }
void IRCanonicalizer::operator()(StatementIR &ir) { convert(ir); }

void IRCanonicalizer::convert(IR &ir) {
    std::visit(util::overload {
        [&](CompUnitIR &node) {},
        [&](FuncDeclIR &node) {},
        [&](ExpressionIR &node) { return convert(node); },
        [&](StatementIR &node) { return convert(node); }
    }, ir);
}

void IRCanonicalizer::convert(ExpressionIR &ir) {
    std::visit(util::overload {
        [&](BinOpIR &node) {},
        [&](CallIR &node) {},
        [&](ConstIR &node) { return; },
        [&](ESeqIR &node) {},
        [&](MemIR &node) {},
        [&](NameIR &node) { return; },
        [&](TempIR &node) { return; }
    }, ir);
}

void IRCanonicalizer::convert(StatementIR &ir) {
    std::visit(util::overload {
        [&](CJumpIR &node) {},
        [&](ExpIR &node) {},
        [&](JumpIR &node) {},
        [&](LabelIR &node) { return; },
        [&](MoveIR &node) {},
        [&](ReturnIR &node) {},
        [&](SeqIR &node) {},
        [&](CallIR &node) { THROW_CompilerError("Call IR should not be considered a statement before conversion"); }
    }, ir);
}
