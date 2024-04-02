#pragma once

#include <string>
#include <list>
#include <vector>
#include <variant>

class Tile;

using Instruction = std::variant<std::string, Tile*>;

// A sequence of instructions that implements a subtree of the IR AST.
//
// Specified in terms of assembly instructions and other tiles.
class Tile {
    // The instructions that implement this tile
    std::vector<Instruction> instructions;

    // The cost of the tile
    int cost;
    // Whether the cost has already been calculated, and doesn't need to be recalculated
    bool cost_calculated;

    // The cost function for calculating cost of a tile
    void calculateCost();
  public:
    // If this is an expression tile, the abstract register results are stored into
    std::string abstract_register = "";

    // Replace the abstract register in this tile's instructions with reg
    Tile& withAbstractRegister(std::string& reg);

    // Get cost of the tile
    int getCost();

    // Add an instruction to the tile, which sets the cost to uncalculated
    void add_instruction(Instruction);

    // Get the full assembly instructions for this tile, expanding other tiles it uses
    std::list<std::string> getFullInstructions();

    // Construct tile with instructions
    Tile(std::vector<Instruction>);
    Tile(std::vector<Instruction>, std::string abstract_reg);

    // Default construct max cost tile
    Tile();
};
