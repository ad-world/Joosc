#pragma once

#include <string>
#include <list>
#include <vector>
#include <variant>

class Tile;

using Instruction = std::variant<std::string, Tile*>;

class Tile {
    std::vector<Instruction> instructions; // The instructions that implement this tile

    int cost = 0; // The cost of the tile
    bool cost_calculated = false; // Whether the cost has already been calculated, and doesn't need to be recalculated

    void calculateCost(); // The cost function for calculating cost of a tile
  public:
    std::string abstract_register; // If this is an expression tile, the abstract register results are stored into

    Tile& withAbstractRegister(std::string& reg); // Replace the abstract register in this tile's instructions with reg

    int getCost(); // Get cost of the tile

    std::list<std::string> getFullInstructions(); // Get the full assembly instructions for this tile, expanding other tiles it uses
};
