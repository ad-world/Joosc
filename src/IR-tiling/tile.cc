#include "tile.h"

#include "exceptions/exceptions.h"
#include "utillities/overload.h"

#include <limits>

void Tile::calculateCost() {
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

Tile::Tile(std::vector<Instruction> instructions) 
    : instructions{std::move(instructions)}, cost{0}, cost_calculated{false} {}

Tile::Tile() 
    : instructions{}, cost{std::numeric_limits<int>::max()}, cost_calculated{true} {}
