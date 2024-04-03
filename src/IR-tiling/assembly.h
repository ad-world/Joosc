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

    static const inline std::string REG8L_ACCUM = "al";
    static const inline std::string REG8H_ACCUM = "ah";

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

    static std::string JumpIfNZ(std::string target) {
        return "jnz " + target;
    }

    static std::string Mov(std::string target, std::string source) {
        return "mov " + target + ", " + source;
    }
    static std::string Mov(std::string target, int source) {
        return Mov(target, std::to_string(source));
    }

    static std::string Lea(std::string target, std::string source) {
        return "lea " + target + ", " + source;
    }

    static std::string Add(std::string target, std::string arg2) {
        return "add " + target + ", " + arg2;
    }
    static std::string Add(std::string target, int arg2) {
        return Add(target, std::to_string(arg2));
    }

    static std::string IMul(std::string multiplicand) {
        return "imul " + multiplicand;
    }

    static std::string IDiv(std::string divisor) {
        return "idiv " + divisor;
    }

    static std::string Xor(std::string destination, std::string other) {
        return "xor " + destination + ", " + other;
    }

    static std::string And(std::string destination, std::string mask) {
        return "and " + destination + ", " + mask;
    }

    static std::string Or(std::string destination, std::string addend) {
        return "or " + destination + ", " + addend;
    }

    static std::string SetZ(std::string destination) {
        return "setz " + destination;
    }

    static std::string SetNZ(std::string destination) {
        return "setnz " + destination;
    }

    static std::string SetL(std::string destination) {
        return "setl " + destination;
    }

    static std::string SetG(std::string destination) {
        return "setg " + destination;
    }

    static std::string SetLE(std::string destination) {
        return "setle " + destination;
    }

    static std::string SetGE(std::string destination) {
        return "setge " + destination;
    }

    static std::string MovZX(std::string destination, std::string arg) {
        return "movzx " + destination + ", " + arg;
    }

    static std::string Cmp(std::string arg1, std::string arg2) {
        return "cmp " + arg1 + ", " + arg2;
    }

    static std::string Test(std::string arg1, std::string arg2) {
        return "test " + arg1 + ", " + arg2;
    }

    static std::string Ret(unsigned int bytes = 0) {
        return bytes > 0 ? "ret " + std::to_string(bytes) : "ret";
    }

    static std::string Push(std::string arg) {
        return "push " + arg;
    }

    static std::string Pop(std::string arg) {
        return "pop " + arg;
    }

    static std::string Call(std::string arg) {
        return "call " + arg;
    }

    // Other helpers

    // Create [base + (index * scale) + displacement] effective address
    static std::string MakeAddress(
        std::string base_register, 
        std::string index_register = "",
        int scale = 1,
        int displacement = 0
    );
};
