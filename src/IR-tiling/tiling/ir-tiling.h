#pragma once

#include "IR/ir.h"
#include "tile.h"
#include <string>
#include <unordered_map>
#include <list>

class RegisterAllocator;

// Convert Canonical IR to x86 assembly
//
// Uses the Optimal Tiling Algorithm, with memoization.
class IRToTilesConverter {
    static size_t abstract_reg_count;
    static std::string newAbstractRegister() { return "%_ABSTRACT_REG" + std::to_string(abstract_reg_count++) + "%"; }

    // Holds the computed best tile for the subtree rooted at every IR in the IR AST.
    // The best tile for each subtree is computed at most once.
    std::unordered_map<ExpressionIR*, Tile> expression_memo;
    std::unordered_map<StatementIR*, Tile> statement_memo;

    // Assign a tile to the subtree, if it is better than the currently assigned tile.
    void decideIsCandidate(ExpressionIR& ir, Tile candidate);
    void decideIsCandidate(StatementIR& ir, Tile candidate); 

    // Escape the temporary, so no temporary is a substring of another
    inline std::string escapeTemporary(const std::string& s) { return "%" + s + "%"; }

  public:
    // Tile the expression, producing the lowest cost tile
    // Generates instructions that store the result in abstract_reg
    ExpressionTile tile(ExpressionIR& node, const std::string& abstract_reg);

    // Tile the statement, producing the lowest cost tile
    // Generates instructions that implement the statement
    StatementTile tile(StatementIR& node);
};
