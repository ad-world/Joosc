#include "simple-reg-alloc.h"
#include <cassert>

void RegAlloc::allocReg(std::string reg) {
    if (reg_offsets.find(reg) == reg_offsets.end()) {
        stack_offset += 4;
        reg_offsets.emplace(reg, stack_offset);
    }
}

int RegAlloc::getOffset(std::string reg) {
    auto it = reg_offsets.find(reg);
    if (it != reg_offsets.end()) {
        return it->second;
    }
    // Error if register not found
    return -1;
}

std::vector<std::string> RegAlloc::prologue(std::vector<std::string> abstract_regs) {
    std::vector<std::string> abs_to_concrete;
    std::vector<std::string> concrete_regs = {"EAX", "ECX", "EDX"};
    int i = 0;
    for(const auto& reg: abstract_regs) {
        assert(i < 3); // Only 3 registers are supported (EAX, ECX, EDX)
        std::string concrete_reg = concrete_regs[i++];
        allocReg(reg);
        int stack_offset = getOffset(reg);
        std::string asm_instruction = "mov " + concrete_reg + ", [ebp - " + std::to_string(stack_offset) + "]";
        abs_to_concrete.push_back(asm_instruction);
    }
    return abs_to_concrete;
}

std::vector<std::string> RegAlloc::epilogue(std::vector<std::string> abstract_regs) {
    std::vector<std::string> abs_to_concrete;
    std::vector<std::string> concrete_regs = {"EAX", "ECX", "EDX"};
    int i = 0;
    for(const auto& reg: abstract_regs) {
        assert(i < 3); // Only 3 registers are supported (EAX, ECX, EDX)
        std::string concrete_reg = concrete_regs[i++];
        int stack_offset = getOffset(reg);
        std::string asm_instruction = "mov [ebp - " + std::to_string(stack_offset) + "], " + concrete_reg;
        abs_to_concrete.push_back(asm_instruction);
    }
    return abs_to_concrete;
}

// this function goes through the output of tiling and replaces all abstract registers with correct stack offsets and implements proglogue and epilogue when required
std::vector<std::string> concreteAsm(std::list<std::string> asm_instructions) {
}