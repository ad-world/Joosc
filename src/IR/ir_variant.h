#pragma once

#include <variant>

using ExpressionIR = std::variant<
    struct BinOpIR,
    struct CallIR,
    struct ConstIR, 
    struct ESeqIR,
    struct MemIR,
    struct NameIR,
    struct TempIR
>;

using StatementIR = std::variant<
    struct CJumpIR,
    struct ExpIR,
    struct JumpIR,
    struct LabelIR,
    struct MoveIR,
    struct ReturnIR,
    struct SeqIR,
    struct CallIR
>;

using IR = std::variant<ExpressionIR, StatementIR, struct FuncDeclIR, struct CompUnitIR>;

