#include "ir-tiling.h"
#include "utillities/util.h"
#include "exceptions/exceptions.h"

#include "assembly.h"

std::list<std::string> IRToTilesConverter::tile(IR &ir) {
    return std::visit(util::overload {
        // For each function, tile each statement in the function body, and make a label for the function call
        [&](CompUnitIR &node) {
            std::list<std::string> output;
            for (auto& func : node.getFunctionList()) {
                output.push_back(Assembly::Label(func->getName()));

                auto& body_tile = tile(func->getBody());
                for (auto& body_instruction : body_tile.getFullInstructions()) {
                    output.push_back(body_instruction);
                }
            }
            return output;
        },
        [&](auto &node) -> std::list<std::string> { THROW_CompilerError("This should not happen"); }
    }, ir);
}

Tile& IRToTilesConverter::tile(ExpressionIR &ir, std::string &abstract_reg) {
    Tile& optimal_tile = uninitialized_tile;

    std::visit(util::overload {
        // [&](BinOpIR &node) {},

        // [&](ConstIR &node) {},

        // [&](MemIR &node) {},

        // [&](NameIR &node) {},

        // [&](TempIR &node) {},

        // [&](ESeqIR &node) {
        //     THROW_CompilerError("ESeqIR should not exist after canonicalization"); 
        // },

        // [&](CallIR &node) {
        //     THROW_CompilerError("CallIR should not be considered an expression after canonicalization"); 
        // }

        [&](auto &node) { THROW_CompilerError("Unimplemented"); }
    }, ir);

    return optimal_tile;
}

Tile& IRToTilesConverter::tile(StatementIR &ir) {
    Tile& optimal_tile = uninitialized_tile;
    
    std::visit(util::overload {
        // [&](CJumpIR &node) {},

        // [&](JumpIR &node) {},

        // [&](LabelIR &node) {},

        // [&](MoveIR &node) {},

        // [&](ReturnIR &node) {},

        // [&](CallIR &node) {},

        // [&](ExpIR &node) {
        //     THROW_CompilerError("ExpIR should not exist after canonicalization"); 
        // },

        // [&](SeqIR &node) {
        //     THROW_CompilerError("Nested SeqIR should not exist after canonicalization"); 
        // }

        [&](auto &node) { THROW_CompilerError("Unimplemented"); }
    }, ir);

    return optimal_tile;
}
