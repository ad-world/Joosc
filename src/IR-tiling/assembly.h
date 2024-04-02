#pragma once

#include <string>

// Helper methods for creating x86 assembly instructions
class Assembly {
    static size_t abstract_reg_count;
  public:
    // Registers
    static std::string newAbstractRegister() { return "ABSTRACT_REG" + (abstract_reg_count++); }

    static inline std::string REG_EAX = "eax";
    static inline std::string REG_ECX = "ecx";
    static inline std::string REG_EDX = "edx";

    // Instructions
    static std::string Label(std::string& name) {
        return name + ":";
    }

    static std::string Jump(std::string& target) {
        return "jmp " + target;
    }

    static std::string Mov(std::string target, std::string source) {
        return "mov " + target + ", " + source;
    }
};
