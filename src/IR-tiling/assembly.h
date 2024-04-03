#pragma once

#include <string>
#include <cmath>

#include "exceptions/exceptions.h"

// Helper methods for creating x86 assembly instructions
class Assembly {
    static size_t abstract_reg_count;
  public:
    // Registers
    static std::string newAbstractRegister() { return "ABSTRACT_REG" + (abstract_reg_count++); }

    static const inline std::string REG32_ACCUM = "eax";
    static const inline std::string REG32_BASE = "ebx";
    static const inline std::string REG32_COUNTER = "ecx";
    static const inline std::string REG32_DATA = "edx";

    static const inline std::string REG32_SP = "esp"; // Stack pointer
    static const inline std::string REG32_BP = "ebp"; // Stack frame pointer

    static const inline std::string REG32_SOURCE = "esi";
    static const inline std::string REG32_DEST = "edi";

    static const inline std::string REG32_IP = "eip"; // Instruction pointer

    // Instructions
    static std::string Label(std::string name) {
        return name + ":";
    }

    static std::string Jump(std::string target) {
        return "jmp " + target;
    }

    static std::string Mov(std::string target, std::string source) {
        return "mov " + target + ", " + source;
    }

    static std::string Lea(std::string target, std::string source) {
        return "lea " + target + ", " + source;
    }

    // Other helpers

    // Create [base + (index * scale) + displacement] effective address
    static std::string MakeAddress(
        std::string base_register, 
        std::string index_register,
        int scale = 1,
        int displacement = 0
    );
};
