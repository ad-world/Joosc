#include "tile.h"

#include "exceptions/exceptions.h"
#include "utillities/overload.h"

#include <limits>
#include <regex>

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
                tile.first->assignAbstract(tile.second);
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

void Tile::add_instructions_after(std::vector<Instruction> after_instructions) {
    cost_calculated = false; // Cost must be recalculated
    for (auto &instr : after_instructions) {
        this->instructions.push_back(instr);
    }
}

void Tile::add_instructions_before(std::vector<Instruction> before_instructions) {
    cost_calculated = false; // Cost must be recalculated
    for (auto &instr : this->instructions) {
        before_instructions.push_back(instr);
    }
    this->instructions = before_instructions;
}

void Tile::assignAbstract(std::string reg) {
    for (auto& instr : this->instructions) {
        std::visit(util::overload {
            [&](AssemblyInstruction& asmb) {
                asmb = std::regex_replace(asmb, std::regex(Tile::ABSTRACT_REG), reg);
            },
            [&](StatementTile tile) {},
            [&](ExpressionTile tile) {
                if (tile.second == reg) {
                    tile.first->assignAbstract(reg);
                }
            }
        }, instr);
    }
}

Tile Tile::assignAbstractToCopy(std::string reg) {
    Tile copy_tile = *this;
    copy_tile.assignAbstract(reg);
    return copy_tile;
}

ExpressionTile Tile::pairWith(std::string abstract_reg) {
    return std::make_pair(this, abstract_reg);
}

Tile::Tile(std::vector<Instruction> instructions) 
    : instructions{std::move(instructions)}, cost{0}, cost_calculated{false} {}

Tile::Tile() 
    : instructions{}, cost{std::numeric_limits<int>::max()}, cost_calculated{true} {}
