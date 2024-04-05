#include "tile.h"

#include "exceptions/exceptions.h"
#include "utillities/overload.h"

#include <limits>

void Tile::calculateCost() {
    // Cost is defined as the number of assembly instructions used to implement tile
    cost = 0;
    for (auto& instr : instructions) {
        std::visit(util::overload {
            [&](AssemblyInstruction&) {
                ++cost;
            },
            [&](StatementTile tile) {
                cost += tile->getCost();
            },
            [&](ExpressionTile tile) {
                cost += tile.first->getCost();
            }
        }, instr);
    }
}

int Tile::getCost() {
    if (!cost_calculated) {
        cost = 0;
        calculateCost();
        cost_calculated = true;
    };

    return cost;
}

std::list<std::string> Tile::getFullInstructions() {
    std::list<std::string> output;

    for (auto& instr : instructions) {
        std::visit(util::overload {
            [&](AssemblyInstruction& asmb) {
                output.push_back(asmb);
            },
            [&](StatementTile tile) {
                for (auto& sub_instr : tile->getFullInstructions()) {
                    output.push_back(sub_instr);
                }
            },
            [&](ExpressionTile tile) {
                for (auto& sub_instr : tile.first->getFullInstructions()) {
                    output.push_back(sub_instr);
                }
            }
        }, instr);
    }

    return output;
}

void Tile::add_instruction(Instruction instr) {
    cost_calculated = false; // Cost must be recalculated
    instructions.push_back(instr);
}

void Tile::add_instructions_after(std::vector<Instruction> instructions) {
    cost_calculated = false; // Cost must be recalculated
    for (auto &instr : instructions) {
        this->instructions.push_back(instr);
    }
}

void Tile::add_instructions_before(std::vector<Instruction> instructions) {
    cost_calculated = false; // Cost must be recalculated
    for (auto &instr : this->instructions) {
        instructions.push_back(instr);
    }
    this->instructions = instructions;
}

ExpressionTile Tile::pairWith(std::string abstract_reg) {
    return std::make_pair(this, abstract_reg);
}

Tile::Tile(std::vector<Instruction> instructions) 
    : instructions{std::move(instructions)}, cost{0}, cost_calculated{false} {}

Tile::Tile() 
    : instructions{}, cost{std::numeric_limits<int>::max()}, cost_calculated{true} {}
