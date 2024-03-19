#include "IR/ir.h"
#include <string>

class ESeqIR {
    StatementIR* stmt;
    ExpressionIR *expr;

public:
    ESeqIR(StatementIR* stmt, ExpressionIR *expr) : stmt(stmt), expr(expr) {}
    StatementIR* stmt() { return stmt; }
    ExpressionIR* expr() { return expr; }
    std::string label() { return "ESEQ"; }
};