#include "tile.h"

#include "exceptions/exceptions.h"
#include "utillities/overload.h"

#include <limits>

void Tile::calculateCost() {
    // Cost is defined as the number of assembly instructions used to implement tile
    cost = 0;
    for (auto& instr : instructions) {
        std::visit(util::overload {
            [&](std::string&) {
                ++cost;
            },
            [&](Tile* tile) {
                cost += tile->getCost();
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

Tile& Tile::withAbstractRegister(std::string& reg) {
    // TODO : replace use in "instructions" with reg
    abstract_register = reg;
    return *this;
}

std::list<std::string> Tile::getFullInstructions() {
    std::list<std::string> output;

    for (auto& instr : instructions) {
        std::visit(util::overload {
            [&](std::string& asmb) {
                output.push_back(asmb);
            },
            [&](Tile* tile) {
                for (auto& sub_instr : tile->getFullInstructions()) {
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

Tile::Tile(std::vector<Instruction> instructions) 
    : instructions{std::move(instructions)}, cost{0}, cost_calculated{false} {}

Tile::Tile(std::vector<Instruction> instructions, std::string abstract_reg) 
    : instructions{std::move(instructions)}, abstract_register{abstract_reg}, cost{0}, cost_calculated{false} {}

Tile::Tile() 
    : instructions{}, cost{std::numeric_limits<int>::max()}, cost_calculated{true} {}
