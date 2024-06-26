#pragma once

#include <string>
#include <cmath>

#include "exceptions/exceptions.h"

// Helper methods for creating x86 assembly instructions
class Assembly {
  public:
    // 8 bit general purpose registers
    static const inline std::string REG8L_ACCUM = "al";
    static const inline std::string REG8H_ACCUM = "ah";

    static const inline std::string REG8L_BASE = "bl";
    static const inline std::string REG8H_BASE = "bh";

    static const inline std::string REG8L_COUNTER = "cl";
    static const inline std::string REG8H_COUNTER = "ch";

    static const inline std::string REG8L_DATA = "dl";
    static const inline std::string REG8H_DATA = "dh";

    static const inline std::string REG8L_STACKPTR = "spl";
    static const inline std::string REG8L_STACKBASEPTR = "bpl";

    static const inline std::string REG8L_SOURCE = "sil";
    static const inline std::string REG8L_DEST = "dil";

    // 16 bit general purpose registers
    static const inline std::string REG16_ACCUM = "ax";
    static const inline std::string REG16_BASE = "bx";
    static const inline std::string REG16_COUNTER = "cx";
    static const inline std::string REG16_DATA = "dx";

    static const inline std::string REG16_STACKPTR = "sp";
    static const inline std::string REG16_STACKBASEPTR = "bp";

    static const inline std::string REG16_SOURCE = "si";
    static const inline std::string REG16_DEST = "di";

    // 32 bit general purpose registers
    static const inline std::string REG32_ACCUM = "eax";
    static const inline std::string REG32_BASE = "ebx";
    static const inline std::string REG32_COUNTER = "ecx";
    static const inline std::string REG32_DATA = "edx";

    static const inline std::string REG32_STACKPTR = "esp"; // Stack pointer
    static const inline std::string REG32_STACKBASEPTR = "ebp"; // Stack frame pointer

    static const inline std::string REG32_SOURCE = "esi";
    static const inline std::string REG32_DEST = "edi";

    // Instruction pointer register (not a gpr)
    static const inline std::string REG32_IP = "eip";

    // Return true if reg is a real gpr, and false if it is an abstract register
    static bool isRealRegister(std::string reg);

    // Instructions
    static std::string Comment(std::string text) {
        return "; " + text;
    }

    static std::string Label(std::string name) {
        return name + ":";
    }

    static std::string SysCall() {
        return "int 0x80";
    }

    static std::string ExternSymbol(std::string arg) {
        return "extern " + arg;
    }

    static std::string GlobalSymbol(std::string arg) {
        return "global " + arg;
    }

    static std::string Jump(std::string target) {
        return "jmp " + target;
    }

    static std::string Je(std::string target) {
        return "je " + target;
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

    static std::string Sub(std::string target, std::string arg2) {
        return "sub " + target + ", " + arg2;
    }
    static std::string Sub(std::string target, int arg2) {
        return Sub(target, std::to_string(arg2));
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

    static std::string Cdq() {
        return "cdq";
    }

    // Other helpers

    // Create [base + (index * scale) + displacement] effective address
    static std::string MakeAddress(
        std::string base_register, 
        std::string index_register = "",
        int scale = 1,
        int displacement = 0
    );

    static std::string LineBreak(int how_many=1) {
        std::string result = "";
        for (int i = 1; i < how_many; ++i) result += "\n";
        return result;
    }
};
