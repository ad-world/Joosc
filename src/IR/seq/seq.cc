#include "seq.h"
#include "IR/ir.h"

std::unique_ptr<StatementIR> SeqIR::makeEmpty() {
    return makeStmt({});
}

SeqIR::SeqIR(std::vector<StatementIR> stmts) {
    for (auto& stmt : stmts) {
        this->stmts.emplace_back(std::make_unique<StatementIR>(std::move(stmt)));
    }

    this->replaceParent = false;
}

std::unique_ptr<StatementIR> SeqIR::makeStmt(std::vector<std::unique_ptr<StatementIR>> stmts, bool replaceParent) {
    return std::make_unique<StatementIR>(
        std::in_place_type<SeqIR>,
        std::move(stmts),
        replaceParent
    );
}
