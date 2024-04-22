#pragma once

#include <string>
#include <variant>
#include <vector>
#include <unordered_set>

#include "utillities/overload.h"
#include "exceptions/exceptions.h"

// Available x86 addressing modes
struct EffectiveAddress {
    const static inline std::string EMPTY_REG = "";

    std::string base_register = EMPTY_REG;
    std::string index_register = EMPTY_REG;
    int scale = 1;
    int displacement = 0;

    EffectiveAddress(std::string base) : base_register{base} {}

    EffectiveAddress(std::string base, int dis) : 
        base_register{base}, displacement{dis} 
    {}

    EffectiveAddress(std::string base, std::string index) : 
        base_register{base}, index_register{index} 
    {}

    EffectiveAddress(std::string base, std::string index, int scale) : 
        base_register{base}, index_register{index}, scale{scale} 
    {}

    EffectiveAddress(std::string base, std::string index, int scale, int dis) : 
        base_register{base}, index_register{index}, scale{scale}, displacement{dis}
    {}

    std::string toString();
};

// Operand for x86 assembly instructions
//
// Register (real or abstract), effective address, or immediate
struct Operand : public std::variant<EffectiveAddress, std::string, int32_t> {
    std::string toString() {
        return std::visit(util::overload {
            [&](EffectiveAddress &adr) { return adr.toString(); },
            [&](std::string &reg) { return reg; },
            [&](int32_t immediate) { return std::to_string(immediate); }
        }, *this);
    }
};

class AssemblyCommon {

    void useOperands(std::unordered_set<std::string*> &target_set, std::vector<Operand*>& operands) {
        for (auto op : operands) {
            std::visit(util::overload {
                [&](EffectiveAddress &adr) { 
                    // In a memory access calculated using the values of registers,
                    // registers are always read, but they themselves are not written to
                    if (adr.base_register != EffectiveAddress::EMPTY_REG) {
                        read_registers.insert(&adr.base_register);
                        used_registers.insert(&adr.index_register);
                    }
                    if (adr.index_register != EffectiveAddress::EMPTY_REG) {
                        read_registers.insert(&adr.index_register);
                        used_registers.insert(&adr.index_register);
                    }
                },
                [&](std::string &reg) { 
                    target_set.insert(&reg);
                    used_registers.insert(&reg);
                },
                [&](int32_t immediate) {}
            }, *op);
        }
    }

  protected:
    void useWriteOperands(std::vector<Operand*> operands) {
        useOperands(write_registers, operands);
    }

    void useReadOperands(std::vector<Operand*> operands) {
        useOperands(read_registers, operands);
    }

  public:
    std::unordered_set<std::string*> used_registers;    // Used (real or abstract) registers

    std::unordered_set<std::string*> write_registers;   // Subset of used_registers that gets written (needs to be stored if on stack)
    std::unordered_set<std::string*> read_registers;    // Subset of used_registers that gets read (needs to be loaded if on stack)
};


struct Mov : public AssemblyCommon {
    Operand dest, source;

    Mov(Operand dest, Operand src) : dest{dest}, source{src} {
        useWriteOperands({&dest});
        useReadOperands({&src});
    }

    std::string toString() {
        return "mov " + dest.toString() + " " + source.toString();
    }
};


// AssemblyInstruction variant, to treat instructions polymorphically without needing pointers

struct AssemblyInstruction : public std::variant<Mov> {

    std::unordered_set<std::string*>& getUsedRegisters() {
        return std::visit(util::overload {
            [&](auto &x) -> std::unordered_set<std::string*>& { return x.used_registers; }
        }, *this);
    }

    std::unordered_set<std::string*>& getWriteRegisters() {
        return std::visit(util::overload {
            [&](auto &x) -> std::unordered_set<std::string*>& { return x.write_registers; }
        }, *this);
    }

    std::unordered_set<std::string*>& getReadRegisters() {
        return std::visit(util::overload {
            [&](auto &x) -> std::unordered_set<std::string*>& { return x.read_registers; }
        }, *this);
    }

    std::string toString() {
        return std::visit(util::overload {
            [&](auto &x) { return x.toString(); }
        }, *this);
    }

};
