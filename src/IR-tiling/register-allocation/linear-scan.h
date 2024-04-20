#pragma once

#include "register-allocator.h"
#include <unordered_map>
#include <iostream>
#include <vector>
#include <set>

class LinearScanAllocator: public RegisterAllocator {
    std::unordered_map<std::string, std::pair<int, int>> live_intervals;
    int current_stack_offset = 0;
    std::unordered_map<std::string, int> stack_offsets;
    static std::vector<std::string> instruction_registers;
    std::unordered_map<std::string, std::string> register_assignments;
    std::vector<std::string> free_registers;
    std::set<std::string> spills;

    void analyzeLiveIntervals(Tile* function_body, int& instruction_no);
    void updateLiveInterval(const std::string& reg, const int& instruction_no);
    void assignRegisters();
    void releaseRegisters(int current_instruction);
    void spillRegister(const std::string& reg);
    AssemblyInstruction loadFromStack(const std::string& reg);
    AssemblyInstruction storeToStack(const std::string& reg);

    public:
        LinearScanAllocator() {
            free_registers = instruction_registers; // Initially, all registers are free.
        }

        int32_t allocateRegisters(Tile* function_body, int instruction_no = 0) override;

        void print_live_intervals() {
            for(auto i = live_intervals.begin(); i != live_intervals.end(); ++i) 
                std::cout << i->first << " " << i->second.first << " " << i->second.second << std::endl;
        }
};