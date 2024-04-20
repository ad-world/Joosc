#pragma once

#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <filesystem>

#include "IR/ir.h"
#include "IR/ir_visitor.h"
#include "IR-tiling/assembly/assembly.h"
#include "utillities/overload.h"
#include "IR-tiling/tiling/ir-tiling.h"
#include "IR-tiling/register-allocation/brainless-allocator.h"
#include "IR-tiling/register-allocation/linear-scan.h"

#define USED_REG_ALLOCATOR LinearScanAllocator

// Find methods/static fields from other compilation units that need to be linked to the assembly file for this cu
class DependencyFinder : public IRSkipVisitor {
    std::unordered_set<std::string> required_static_fields;
    std::unordered_set<std::string> required_functions;

    CompUnitIR& cu;

  public:
    using IRSkipVisitor::operator();

    explicit DependencyFinder(CompUnitIR& cu) : cu{cu} {
        this->operator()(cu);
        required_functions.insert("__exception");
        required_functions.insert("__malloc");
    }

    virtual void operator()(CallIR &node) override {
        if (auto name = std::get_if<NameIR>(&node.getTarget())) {
            std::string called_function = name->getName();
            if (!(cu.getFunctions().count(called_function) || required_functions.count(called_function))) {
                // Required function is not from this compilation unit already or already included
                required_functions.insert(called_function);
            }
            this->visit_children(node);
            return;
        }
        THROW_CompilerError("Function call target is not a label");
    }

    virtual void operator()(TempIR &node) override {
        if (!required_static_fields.count(node.getName()) && node.isGlobal) {
            // Required static field is not already included
            required_static_fields.insert(node.getName());
        }
        this->visit_children(node); 
    }

    std::unordered_set<std::string> getRequiredStaticFields() { return required_static_fields; }
    std::unordered_set<std::string> getRequiredFunctions() { return required_functions; }
};

// Class in charge of generating all the output assembly for each compilation unit.
//
// Tiles each compilation unit and creates the files as appropriate.
class AssemblyGenerator {
    IRToTilesConverter converter;
  public:
    void generateCode(std::vector<IR>& ir_trees, std::string entrypoint_method) {
        std::vector<std::pair<std::string, Tile>> static_fields;
        
        // Reset output directory
        for (auto& path: std::filesystem::directory_iterator("output")) {
            std::filesystem::remove_all(path);
        }

        // Emit a file for each compilation unit
        size_t file_id = 0;
        for (auto &ir : ir_trees) {
            std::visit(util::overload {
                [&](CompUnitIR& cu) {
                    // Get static fields to dump in .data section of entrypoint file later
                    for (auto& [field_name, field_initalizer] : cu.getFieldList()) {
                        assert(field_initalizer);
                        auto [tile, _] = converter.tile(*field_initalizer, Assembly::REG32_ACCUM);
                        tile->assignAbstract(Assembly::REG32_ACCUM);
                        USED_REG_ALLOCATOR registerAllocator;
                        registerAllocator.allocateRegisters(tile);
                        registerAllocator.print_live_intervals();
                        static_fields.emplace_back(field_name, *tile);
                    }

                    // Generate the assembly file for the compilation unit
                    std::ofstream output_file {"output/cu_" + std::to_string(file_id++) + ".s"};
                    output_file << "section .text\n\n";

                    // Export functions as global
                    for (auto& func : cu.getFunctionList()) {
                        output_file << Assembly::GlobalSymbol(func->getName()) << "\n";
                    }
                    output_file << "\n";

                    // Import required functions/static fields
                    auto dependency_finder = DependencyFinder(cu);
                    for (auto& req_func : dependency_finder.getRequiredFunctions()) {
                        output_file << Assembly::ExternSymbol(req_func) << "\n";
                    }
                    for (auto& req_field : dependency_finder.getRequiredStaticFields()) {
                        output_file << Assembly::ExternSymbol(req_field) << "\n";
                    }
                    output_file << "\n";

                    // Tile each method in the compilation unit
                    for (auto& func : cu.getFunctionList()) {
                        // Function label
                        output_file << Assembly::Label(func->getName()) << "\n";

                        // Function prologue
                        auto body_tile = converter.tile(func->getBody());
                        USED_REG_ALLOCATOR registerAllocator;
                        int32_t stack_size = registerAllocator.allocateRegisters(body_tile);
                        registerAllocator.print_live_intervals();
                        output_file << "\t" << Assembly::Push(Assembly::REG32_STACKBASEPTR)                            << "\n";
                        output_file << "\t" << Assembly::Mov(Assembly::REG32_STACKBASEPTR, Assembly::REG32_STACKPTR)   << "\n";
                        output_file << "\t" << Assembly::Sub(Assembly::REG32_STACKPTR, 4 * stack_size)                 << "\n";

                        // Function body
                        for (auto& body_instruction : body_tile->getFullInstructions()) {
                            output_file << "\t" << body_instruction << "\n";
                        }
                        output_file << "\n";
                    }
                },
                [&](auto&) { THROW_CompilerError("shouldn't happen"); }
            }, ir);
        }

        // Emit a main file for the entrypoint
        std::ofstream start_file {"output/main.s"};
        
        start_file << "section .data\n\n";
        for (auto& [field_name, initalizer_tile] : static_fields) {
            start_file << field_name + ": dd 0" << "\n";
        }
        start_file << "\n"

        << "section .text"                              << "\n\n";

        for (auto& [field_name, initalizer_tile] : static_fields) {
            start_file << Assembly::GlobalSymbol(field_name) << "\n";
        }
        start_file 
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
