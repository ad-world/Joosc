#include "brainless-allocator.h"
#include "IR-tiling/tile.h"
#include "utillities/overload.h"
#include "IR-tiling/assembly.h"
#include "exceptions/exceptions.h"

#include <regex>

std::vector<std::string> BrainlessRegisterAllocator::instruction_registers 
    // = {Assembly::REG32_COUNTER, Assembly::REG32_DATA, Assembly::REG32_ACCUM};
    = {Assembly::REG32_COUNTER, Assembly::REG32_SOURCE, Assembly::REG32_DEST};

void BrainlessRegisterAllocator::findOffsets(Tile* function_body) {
    for (auto& instr : getInstructions(function_body)) {
        std::visit(util::overload {
            [&](AssemblyInstruction& asmb) {},
            [&](StatementTile tile) {
                findOffsets(tile);
            },
            [&](ExpressionTile tile) {
                if (!Assembly::isRealRegister(tile.second) && !reg_offsets.count(tile.second)) {
                    reg_offsets[tile.second] = stack_offset;
                    stack_offset += 4;
                }
                findOffsets(tile.first);
            }
        }, instr);
    }
}

int32_t BrainlessRegisterAllocator::allocateRegisters(Tile* function_body) {
    findOffsets(function_body);

    std::vector<Instruction> new_instructions;

    for (auto& instr : getInstructions(function_body)) {
        std::visit(util::overload {
            [&](AssemblyInstruction& asmb) {
                replaceAbstracts(asmb, new_instructions);
            },
            [&](StatementTile tile) {
                allocateRegisters(tile);
                new_instructions.push_back(tile);
            },
            [&](ExpressionTile tile) {
                if (Assembly::isRealRegister(tile.second)) {
                    allocateRegisters(tile.first);
                    new_instructions.push_back(tile);
                } else {
                    tile.first->assignAbstract(tile.second);
                    allocateRegisters(tile.first);
                    new_instructions.push_back(tile.first);
                }
            }
        }, instr);
    }

    getInstructions(function_body) = new_instructions;

    return reg_offsets.size();
}

Instruction BrainlessRegisterAllocator::loadAbstractRegister(std::string reg_to, std::string abstract_reg) {
    return Assembly::Mov(
        reg_to,
        Assembly::MakeAddress(Assembly::REG32_STACKBASEPTR, "", 1, -1 * reg_offsets[abstract_reg])
    );
}

Instruction BrainlessRegisterAllocator::storeAbstractRegister(std::string abstract_reg, std::string reg_from) {
    return Assembly::Mov(
        Assembly::MakeAddress(Assembly::REG32_STACKBASEPTR, "", 1, -1 * reg_offsets[abstract_reg]),
        reg_from
    );
}

void BrainlessRegisterAllocator::replaceAbstracts(AssemblyInstruction instruction, std::vector<Instruction>& target_vector) {
    // Inefficiently, search for every single abstract register used in the function in every single instruction
    std::vector<AssemblyInstruction> found;
    for (const auto& [reg, offset] : reg_offsets) {
        if (instruction.find(reg) != std::string::npos) {
            found.push_back(reg);
        } 
    }

    if (found.size() > 3) {
        std::string found_registers = "";
        for (auto& reg: found) found_registers += " " + reg;
        THROW_CompilerError(
            "x86 instruction using more than 3 registers? Something is wrong.\n"
            "Instruction: " + instruction + "\n"
            "Found registers:" + found_registers
        );
    }

    for (int i = 0; i < found.size(); ++i) {
        target_vector.push_back(loadAbstractRegister(instruction_registers[i], found[i]));
        instruction = std::regex_replace(instruction, std::regex(found[i]), instruction_registers[i]);
    }
    
    target_vector.push_back(instruction);

    for (int i = 0; i < found.size(); ++i) {
        target_vector.push_back(storeAbstractRegister(found[i], instruction_registers[i]));
    }
}  
