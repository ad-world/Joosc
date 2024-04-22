#pragma once

#include "assembly-refactor.h"

// AssemblyInstruction variant, to treat instructions polymorphically without needing pointers

using AssemblyInstructionInheritedVariant = std::variant<
    Mov
>;

struct AssemblyInstruction : public AssemblyInstructionInheritedVariant {
    using variant::variant;

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
