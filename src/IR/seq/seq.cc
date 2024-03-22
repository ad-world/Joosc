#include "seq.h"
#include "IR/ir.h"

std::unique_ptr<StatementIR> SeqIR::makeStmt(std::vector<std::unique_ptr<StatementIR>> stmts, bool replaceParent) {
    return std::make_unique<StatementIR>(
        std::in_place_type<SeqIR>,
        std::move(stmts),
        replaceParent
    );
}
