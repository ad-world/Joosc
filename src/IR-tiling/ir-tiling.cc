#include "ir-tiling.h"
#include "utillities/util.h"
#include "exceptions/exceptions.h"

std::list<StatementTile> IRToTilesConverter::tile(IR &ir) {
    std::visit(util::overload {
        // For each function, tile each statement in the function body, and make a label for the function call
        [&](CompUnitIR &node) {
            for (auto& func : node.getFunctionList()) {
                
            }
        },
        [&](auto &node) { THROW_CompilerError("This should not happen"); }
    }, ir);
}

ExpressionTile IRToTilesConverter::tile(ExpressionIR &ir, std::string &abstract_reg) {
    ExpressionTile optimal_tile;

    std::visit(util::overload {
        [&](BinOpIR &node) {
            
            if (node.left.isTemporary() && node.right.isTemporary()) {
                // This tile is applicable
            }



        },

        [&](ConstIR &node) {},

        [&](MemIR &node) {},

        [&](NameIR &node) {},

        [&](TempIR &node) {},

        [&](ESeqIR &node) {
            THROW_CompilerError("ESeqIR should not exist after canonicalization"); 
        },

        [&](CallIR &node) {
            THROW_CompilerError("CallIR should not be considered an expression after canonicalization"); 
        }
    }, ir);

    return optimal_tile;
}

StatementTile IRToTilesConverter::tile(StatementIR &ir) {
    StatementTile optimal_tile;
    
    std::visit(util::overload {
        [&](CJumpIR &node) {},

        [&](JumpIR &node) {},

        [&](LabelIR &node) {},

        [&](MoveIR &node) {},

        [&](ReturnIR &node) {},

        [&](CallIR &node) {},

        [&](ExpIR &node) {
            THROW_CompilerError("ExpIR should not exist after canonicalization"); 
        },

        [&](SeqIR &node) {
            THROW_CompilerError("Nested SeqIR should not exist after canonicalization"); 
        }
    }, ir);

    return optimal_tile;
}
