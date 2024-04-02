#pragma once

#include "IR/ir.h"
#include "tile.h"
#include <string>
#include <unordered_map>

// Convert Canonical IR to x86 assembly
//
// Uses the Optimal Tiling Algorithm, with memoization.
class IRToTilesConverter {

    // Holds the computed best tile for the subtree rooted at every IR in the IR AST.
    // The best tile for each subtree is computed at most once.
    std::unordered_map<ExpressionIR*, ExpressionTile> expression_memo;
    std::unordered_map<StatementIR*, StatementTile> statement_memo;

    // Tile the expression, producing the lowest cost tile
    ExpressionTile& tile(ExpressionIR&, std::string&);

    // Tile the statement, producing the lowest cost tile
    StatementTile& tile(StatementIR&);

  public:
    // Tile the whole compilation unit
    std::list<StatementTile> tile(IR&);
};
