#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "IR/ir.h"
#include "IR-tiling/assembly/assembly.h"
#include "utillities/overload.h"
#include "IR-tiling/tiling/ir-tiling.h"
#include "IR-tiling/register-allocation/brainless-allocator.h"

#define USED_REG_ALLOCATOR BrainlessRegisterAllocator()

// Class in charge of generating all the output assembly for each compilation unit.
//
// Tiles each compilation unit and creates the files as appropriate.
class AssemblyGenerator {
    IRToTilesConverter converter;

  public:
    void generateCode(std::vector<IR>& ir_trees, std::string entrypoint_method) {
        std::vector<std::pair<std::string, Tile>> static_fields;

        // Emit a file for each compilation unit
        for (auto &ir : ir_trees) {
            std::visit(util::overload {
                [&](CompUnitIR& cu) {
                    // Get static fields to dump in .data section of entrypoint file later
                    for (auto& [field_name, field_initalizer] : cu.getFieldList()) {
                        assert(field_initalizer);
                        auto [tile, _] = converter.tile(*field_initalizer, Assembly::REG32_ACCUM);
                        tile->assignAbstract(Assembly::REG32_ACCUM);
                        USED_REG_ALLOCATOR.allocateRegisters(tile);
                        static_fields.emplace_back(field_name, *tile);
                    }

                    // // Tile each method in the compilation unit
                    // auto instructions = converter.tile(ir);

                    // // Generate the assembly file for the compilation unit
                    // std::ofstream output_file {"output/cu.s"};
                    // for (auto& instr : instructions) {
                    //     output_file << instr << "\n";
                    // }
                },
                [&](auto&) { THROW_CompilerError("shouldn't happen"); }
            }, ir);
        }

        // Emit a main file for the entrypoint
        std::ofstream start_file {"output/main.s"};
        
        start_file << "section .data\n";
        for (auto& [field_name, initalizer_tile] : static_fields) {
            start_file << "\n";
            start_file << Assembly::GlobalSymbol(field_name) << "\n";
            start_file << Assembly::Label(field_name) << "\n";
        }
        start_file << "\n"

        << "section .text"                              << "\n\n"

        << Assembly::GlobalSymbol("_start")             << "\n"
        << Assembly::ExternSymbol(entrypoint_method)    << "\n\n"

        << Assembly::Label("_start") << "\n"
            << "\t" << Assembly::Comment("Initialize all the static fields of all the compilation units, in order");
            for (auto& [field_name, initalizer_tile] : static_fields) {
                for (auto &instr : initalizer_tile.getFullInstructions()) {
                    start_file << "\t" << instr << "\n";
                }
                start_file << "\t" << Assembly::Mov(Assembly::MakeAddress(field_name), Assembly::REG32_ACCUM) << "\n";
            }
            start_file << "\n"

            << "\t" << Assembly::Comment("Call entrypoint method and execute exit() system call with return value in REG32_BASE") << "\n"
            << "\t" << Assembly::Call(entrypoint_method)                            << "\n"
            << "\t" << Assembly::Mov(Assembly::REG32_BASE, Assembly::REG32_ACCUM)   << "\n"
            << "\t" << Assembly::Mov(Assembly::REG32_ACCUM, 1)                      << "\n"
            << "\t" << Assembly::SysCall()                                          << "\n"
        ;
    }
};
