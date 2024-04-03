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

    static inline std::string REG_EAX = "eax";
    static inline std::string REG_EBX = "ebx";
    static inline std::string REG_ECX = "ecx";
    static inline std::string REG_EDX = "edx";

    static inline std::string REG_ESP = "esp"; // Stack pointer
    static inline std::string REG_EBP = "ebp"; // Stack frame pointer

    static inline std::string REG_ESI = "esi";
    static inline std::string REG_EDI = "edi";

    static inline std::string REG_EIP = "eip"; // Instruction pointer

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
