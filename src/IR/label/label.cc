#include "label.h"
#include "IR/ir.h"

size_t LabelIR::num_labels = 0;
 std::unique_ptr<StatementIR> LabelIR::makeStmt(std::string str) {
    return std::make_unique<StatementIR>(std::in_place_type<LabelIR>, str);
}
