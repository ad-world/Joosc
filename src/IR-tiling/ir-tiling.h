#pragma once

#include "IR/ir.h"
#include <vector>
#include <list>
#include <memory>
#include <string>

struct ExpressionTile {
    std::string reg; // Abstract register the result is stored in
    
    std::list<std::string> instructions; // The assembly instructions that implement this tile
    int cost; // The cost of the tile, under our cost model
};

struct StatementTile {
    std::list<std::string> instructions; // The assembly instructions that implement this tile
    int cost; // The cost of the tile, under our cost model
};

// Convert Canonical IR to x86 assembly
class IRToTilesConverter {

  public:
    // Tile the whole compilation unit
    std::list<StatementTile> tile(IR&);

    // Tile the expression
    ExpressionTile tile(ExpressionIR&);

    // Tile the statement
    StatementTile tile(StatementIR&);
};
