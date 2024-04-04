#pragma once

#include "IR/ir.h"
#include "tile.h"
#include <string>
#include <unordered_map>
#include <list>

// Convert Canonical IR to x86 assembly
//
// Uses the Optimal Tiling Algorithm, with memoization.
class IRToTilesConverter {

    // Holds the computed best tile for the subtree rooted at every IR in the IR AST.
    // The best tile for each subtree is computed at most once.
    std::unordered_map<ExpressionIR*, Tile> expression_memo;
    std::unordered_map<StatementIR*, Tile> statement_memo;

    // Assign a tile to the subtree, if it is better than the currently assigned tile.
    void decideIsCandidate(ExpressionIR& ir, Tile candidate);
    void decideIsCandidate(StatementIR& ir, Tile candidate); 

    // Tile the expression, producing the lowest cost tile
    // Generates instructions that store the result in abstract_reg
    Tile* tile(ExpressionIR& node, std::string& abstract_reg);

    // Tile the statement, producing the lowest cost tile
    // Generates instructions that implement the statement
    Tile* tile(StatementIR& node);

  public:
    // Tile the whole compilation unit, producing a list of assembly instructions
    // Each element in the list returned is an assembly instruction
    std::list<std::string> tile(IR& node);
};