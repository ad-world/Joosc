#pragma once

#include <string>
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

    // Other helpers
    // Create [base + (index * scale) + displacement] addressing mode access
    static std::string MakeAddress(
        std::string base_register, 
        std::string index_register,
        int scale = 1,
        int displacement = 0
    ) {
        if (!(scale == 1 || scale == 2 || scale == 4 || scale == 8)) {
            THROW_CompilerError("Invalid parameter for scale");
        }

        std::string result = base_register + " + ";

        if (scale != 1) {
            result += "(" + index_register + " * " + std::to_string(scale) + ")";
        } else {
            result += index_register;
        }

        if (displacement != 0) {
            result += " + " + std::to_string(displacement);
        }

        return "[" + result + "]";
    }

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
};
