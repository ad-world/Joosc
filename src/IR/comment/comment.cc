#include "comment.h"
#include "IR/ir.h"

std::unique_ptr<StatementIR> CommentIR::makeStmt(std::string text) {
    return std::make_unique<StatementIR>(
        std::in_place_type<CommentIR>,
        std::move(text)
    );
}
