#pragma once

#include <variant>

using ExpressionIR = std::variant<
    struct ConstIR, 
    struct TempIR,
    struct BinOpIR,
    struct MemIR,
    struct CallIR,
    struct NameIR
    struct ESeqIR
>;

using StatementIR = std::variant<
    struct MoveIR,
    struct ExpIR,
    struct SeqIR,
    struct JumpIR,
    struct CJumpIR,
    struct LabelIR,
    struct ReturnIR
>;

using IR = std::variant<ExpressionIR, StatementIR, struct FuncDeclIR, struct CompUnitIR>;
