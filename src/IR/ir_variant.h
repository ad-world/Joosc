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
    struct JumpIR,
    struct LabelIR,
    struct MoveIR,
    struct ReturnIR,
    struct SeqIR
    // struct ExpIR, (TODO what is this?)
>;

using IR = std::variant<ExpressionIR, StatementIR, struct FuncDeclIR, struct CompUnitIR>;

