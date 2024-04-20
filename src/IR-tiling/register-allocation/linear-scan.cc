#include "linear-scan.h"
#include "utillities/overload.h"
#include "IR-tiling/assembly/assembly.h"
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <set>

std::vector<std::string> LinearScanAllocator::instruction_registers 
    = {Assembly::REG32_ACCUM, Assembly::REG32_BASE, Assembly::REG32_COUNTER, Assembly::REG32_DATA,
        Assembly::REG32_SOURCE, Assembly::REG32_DEST};

void LinearScanAllocator::analyzeLiveIntervals(Tile* function_body, int& instruction_no) {
    for (auto& instr : getInstructions(function_body)) {
        std::visit(util::overload {
            [&](AssemblyInstruction& asmb) {},
            [&](StatementTile tile) {
                ++instruction_no;
                analyzeLiveIntervals(tile, instruction_no);
            },
            [&](ExpressionTile tile) {
                ++instruction_no;
                updateLiveInterval(tile.second, instruction_no);
                analyzeLiveIntervals(tile.first, instruction_no);
            }
        }, instr);
    }
}

int32_t LinearScanAllocator::allocateRegisters(Tile* function_body, int instruction_no) {
    analyzeLiveIntervals(function_body, instruction_no);
    for (auto& instr : getInstructions(function_body)) {
        std::visit(util::overload {
            [&](AssemblyInstruction& asmb) {},
            [&](StatementTile tile) {
                allocateRegisters(tile, instruction_no);
            },
            [&](ExpressionTile tile) {
                allocateRegisters(tile.first, instruction_no);
            }
        }, instr);
    }
    assignRegisters();
    return current_stack_offset / 4;
}

void LinearScanAllocator::updateLiveInterval(const std::string& reg, const int& instruction_no) {
    auto& interval = live_intervals[reg];
    if (interval.first == 0 && interval.second == 0) {
        interval.first = instruction_no;
    }
    interval.second = std::max(instruction_no, interval.second);
}

void LinearScanAllocator::assignRegisters() {
    std::vector<std::pair<std::string, std::pair<int, int>>> sorted_intervals(live_intervals.begin(), live_intervals.end());
    std::sort(sorted_intervals.begin(), sorted_intervals.end(), [](const auto& a, const auto& b) {
        return a.second.first < b.second.first;
    });

    // Store instructions for loading and storing registers
    std::vector<AssemblyInstruction> load_store_instructions;
    for (auto& reg_interval : sorted_intervals) {
        releaseRegisters(reg_interval.second.first);

        // Check if the register is previously spilled and needs to be reloaded
        if (spills.find(reg_interval.first) != spills.end() && !free_registers.empty()) {
            // Generate load instruction since it was spilled
            auto load_instr = loadFromStack(reg_interval.first);
            load_store_instructions.push_back(load_instr);
        }

        if (free_registers.empty()) {
            spillRegister(reg_interval.first);
            // Reload register from stack if needed for use
            if (live_intervals[reg_interval.first].first <= reg_interval.second.second) {
                auto load_instr = loadFromStack(reg_interval.first);
                load_store_instructions.push_back(load_instr);
            }
        } else {
            register_assignments[reg_interval.first] = free_registers.back();
            free_registers.pop_back();
        }
    }

    // Generate store instructions for all spilled registers
    for (const auto& reg : register_assignments) {
        if (spills.find(reg.first) != spills.end()) {
            auto store_instr = storeToStack(reg.first);
            load_store_instructions.push_back(store_instr);
        }
    }
}

// Release registers that are no longer in use
void LinearScanAllocator::releaseRegisters(int current_instruction) {
    for (auto it = register_assignments.begin(); it != register_assignments.end(); ) {
        auto reg = it->first;
        auto& interval = live_intervals[reg];
        if (interval.second < current_instruction) {
            free_registers.push_back(it->second);
            it = register_assignments.erase(it);
        } else {
            ++it;
        }
    }
}

// Spilling abstract register to stack
void LinearScanAllocator::spillRegister(const std::string& reg) {
    if (spills.find(reg) == spills.end()) {
        spills.insert(reg);
        stack_offsets[reg] = current_stack_offset;
        current_stack_offset += 4;
    }
}

AssemblyInstruction LinearScanAllocator::loadFromStack(const std::string& reg) {
    int offset = stack_offsets[reg];
    return Assembly::Mov(reg, Assembly::MakeAddress(Assembly::REG32_STACKBASEPTR, "", 1, -offset));
}

AssemblyInstruction LinearScanAllocator::storeToStack(const std::string& reg) {
    int offset = stack_offsets[reg];
    return Assembly::Mov(Assembly::MakeAddress(Assembly::REG32_STACKBASEPTR, "", 1, -offset), reg);
}