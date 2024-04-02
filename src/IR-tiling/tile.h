#pragma once

#include <string>
#include <list>
#include <vector>

class Tile;

using Instruction = std::variant<std::string, Tile*>;

class Tile {
    std::vector<Tile*> preceding_tiles;
    std::list<std::string> instructions; // The assembly instructions that implement this tile, after all preceding tiles

    std::vector<Instruction> instructions; // The instructions that implement this tile

    int cost; // The cost of the tile
    bool cost_calculated = false; // Whether the cost has already been calculated, and doesn't need to be recalculated

    int calculateCost();
  public:
    std::string abstract_register; // If this is an expression tile, the abstract register results are stored into

    Tile& withAbstractRegister(std::string& reg); // Replace the abstract register in this tile's instructions with reg

    int getCost(); // Get cost of the tile

    std::list<std::string> getFullInstructions(); // Get the full instructions for this tile, expanding other tiles it uses
}

struct ExpressionTile {
    std::string reg; // Abstract register the result is stored in
    
    std::list<std::string> instructions; // The assembly instructions that implement this tile
    int cost; // The cost of the tile, under our cost model
};

struct StatementTile {
    std::list<std::string> instructions; // The assembly instructions that implement this tile
    int cost; // The cost of the tile, under our cost model
};
