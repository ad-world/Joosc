#pragma once

#include "register-allocator.h"

#include <unordered_map>
#include <vector>

// Register allocator that spills all variables to the stack.
class BrainlessRegisterAllocator : public RegisterAllocator {
    std::unordered_map<std::string, int> reg_offsets;
    int stack_offset = 4;

    // Registers designated to load stack values into
    static std::vector<std::string> instruction_registers;

    // Generate code to load the abstract register
    Instruction loadAbstractRegister(std::string reg_to, std::string abstract_reg);

    // Generate code to store the abstract register
    Instruction storeAbstractRegister(std::string abstract_reg, std::string reg_from);

    // Pick a stack offset for each abstract register, and place in map
    void findOffsets(Tile* function_body);

    // Generate code to load all abstract registers into real registers, and replace the use of abstracts with reals
    void replaceAbstracts(AssemblyInstruction instruction, std::vector<Instruction>& target_vector);

  public:
    int32_t allocateRegisters(Tile* function_body) override;
};
