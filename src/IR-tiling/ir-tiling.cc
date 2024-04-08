#include "ir-tiling.h"
#include "utillities/util.h"
#include "exceptions/exceptions.h"
#include "assembly.h"
#include "register-allocation/register-allocator.h"
#include "IR/code-gen-constants.h"

#include <regex>

size_t IRToTilesConverter::abstract_reg_count = 0;

void IRToTilesConverter::decideIsCandidate(ExpressionIR& ir, Tile candidate) {
    Tile& optimal_tile = expression_memo[&ir];
    if (candidate.getCost() < optimal_tile.getCost()) {
        optimal_tile = candidate;
    }
};

void IRToTilesConverter::decideIsCandidate(StatementIR& ir, Tile candidate) {
    Tile& optimal_tile = statement_memo[&ir];
    if (candidate.getCost() < optimal_tile.getCost()) {
        optimal_tile = candidate;
    }
};

std::list<std::string> IRToTilesConverter::tile(IR &ir) {
    return std::visit(util::overload {
        // For each function, tile each statement in the function body, and make a label for the function call
        [&](CompUnitIR &node) {
            std::list<std::string> output;

            for (auto& func : node.getFunctionList()) {
                // Function label
                if (func->getName() == entrypoint_method) {
                    // Add global start symbol for program execution entrypoint
                    current_is_entrypoint = true;
                    output.push_back(Assembly::GlobalSymbol(Assembly::StartLabel));
                    output.push_back(Assembly::StartLabel);
                } else {
                    current_is_entrypoint = false;
                }
                output.push_back(Assembly::Label(func->getName()));

                auto body_tile = tile(func->getBody());

                int32_t stack_size = register_allocator->allocateRegisters(body_tile);

                // Function prologue
                output.push_back(Assembly::Push(Assembly::REG32_STACKBASEPTR));
                output.push_back(Assembly::Mov(Assembly::REG32_STACKBASEPTR, Assembly::REG32_STACKPTR));
                output.push_back(Assembly::Sub(Assembly::REG32_STACKPTR, 4 * stack_size));

                // Function body
                for (auto& body_instruction : body_tile->getFullInstructions()) {
                    output.push_back(body_instruction);
                }
            }

            return output;
        },
        [&](auto &node) -> std::list<std::string> { THROW_CompilerError("This should not happen"); }
    }, ir);
}

ExpressionTile IRToTilesConverter::tile(ExpressionIR &ir, const std::string &abstract_reg) {
    if (expression_memo.count(&ir)) {
        return expression_memo[&ir].pairWith(abstract_reg);
    }

    // Generic tile that must be applicable, if no specialized tiles are applicable
    Tile generic_tile;

    std::visit(util::overload {
        [&](BinOpIR &node) {
            
            // Generic tile that is always applicable : store operands in abstract registers, then do something
            std::string operand1_reg = newAbstractRegister();
            std::string operand2_reg = newAbstractRegister();

            generic_tile = Tile({
                tile(node.getLeft(), operand1_reg),
                tile(node.getRight(), operand2_reg)
            });

            switch (node.op) {
                case BinOpIR::OpType::ADD: {
                    generic_tile.add_instructions_after({
                        Assembly::Lea(Tile::ABSTRACT_REG, Assembly::MakeAddress(operand1_reg, operand2_reg))
                    });
                    break;
                } 
                case BinOpIR::OpType::SUB: {
                    generic_tile.add_instructions_after({
                        Assembly::Sub(operand1_reg, operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, operand1_reg)
                    });
                    break;
                }
                case BinOpIR::OpType::MUL: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg),
                        Assembly::IMul(operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, Assembly::REG32_ACCUM)
                    });
                    break;
                }
                case BinOpIR::OpType::DIV: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg), // Move low 32 bits of dividend into accumulator
                        Assembly::Xor(Assembly::REG32_DATA, Assembly::REG32_DATA), // Clear high 32 bits of dividend
                        Assembly::IDiv(operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, Assembly::REG32_ACCUM) // Quotient stored in ACCUM
                    });
                    break;
                }
                case BinOpIR::OpType::MOD: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg), // Move low 32 bits of dividend into accumulator
                        Assembly::Xor(Assembly::REG32_DATA, Assembly::REG32_DATA), // Clear high 32 bits of dividend
                        Assembly::IDiv(operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, Assembly::REG32_DATA) // Remainder stored in DATA
                    });
                    break;
                }
                case BinOpIR::OpType::AND: {
                    generic_tile.add_instructions_after({
                        Assembly::And(operand1_reg, operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, operand1_reg)
                    });
                    break;
                }
                case BinOpIR::OpType::OR: {
                    generic_tile.add_instructions_after({
                        Assembly::Or(operand1_reg, operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, operand1_reg)
                    });
                    break;
                }
                case BinOpIR::OpType::EQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetZ(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                    break;
                }
                case BinOpIR::OpType::NEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetNZ(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                    break;
                }
                case BinOpIR::OpType::LT: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetL(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                    break;
                }
                case BinOpIR::OpType::GT: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetG(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                    break;
                }
                case BinOpIR::OpType::LEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetLE(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                    break;
                }
                case BinOpIR::OpType::GEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetGE(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                    break;
                }
                default: {
                    THROW_CompilerError("Operation is not supported in Joosc");
                }
            }
        },

        [&](ConstIR &node) {
            // 32-bit immediate
            generic_tile = Tile({
                Assembly::Mov(Tile::ABSTRACT_REG, node.getValue())
            });
        },

        [&](MemIR &node) {
            std::string address_reg = newAbstractRegister();

            generic_tile = Tile({
                tile(node.getAddress(), address_reg),
                Assembly::Lea(Tile::ABSTRACT_REG, Assembly::MakeAddress(address_reg))
            });
        },

        [&](NameIR &node) {
            generic_tile = Tile({
                Assembly::Mov(Tile::ABSTRACT_REG, node.getName())
            });
        },

        [&](TempIR &node) {
            // Special registers : ABSTRACT_RET is REG32_ACCUM
            if (node.getName() == CGConstants::ABSTRACT_RET) {
                generic_tile = Tile({
                    Assembly::Mov(Tile::ABSTRACT_REG, Assembly::REG32_ACCUM)
                });
            }
            // Special registers : ABSTRACT_ARG_PREFIX# is stack offset from caller
            else if (node.getName().rfind(CGConstants::ABSTRACT_ARG_PREFIX, 0) == 0) {
                int arg_num 
                    = std::stoi(std::regex_replace(node.getName(), std::regex(CGConstants::ABSTRACT_ARG_PREFIX), ""));

                generic_tile = Tile({
                    Assembly::Mov(
                        Tile::ABSTRACT_REG, 
                        Assembly::MakeAddress(Assembly::REG32_STACKBASEPTR, "", 1, 4 * (arg_num + 1))
                    )
                });
            }
            // Not a special register
            else {
                generic_tile = Tile({
                    Assembly::Mov(Tile::ABSTRACT_REG, escapeTemporary(node.getName()))
                });
            }
        },

        [&](ESeqIR &node) {
            THROW_CompilerError("ESeqIR should not exist after canonicalization"); 
        },

        [&](CallIR &node) {
            THROW_CompilerError("CallIR should not be considered an expression after canonicalization"); 
        }
    }, ir);

    if (generic_tile.getCost() == Tile().getCost()) {
        THROW_CompilerError("Generic tile was not assigned to " + std::visit([&](auto &x){ return x.label(); }, ir));
    }
    decideIsCandidate(ir, generic_tile);

    return expression_memo[&ir].pairWith(abstract_reg);
}

StatementTile IRToTilesConverter::tile(StatementIR &ir) {
    if (statement_memo.count(&ir)) {
        return &statement_memo[&ir];
    }

    // Generic tile that must be applicable, if no specialized tiles are applicable
    Tile generic_tile;
    
    std::visit(util::overload {
        [&](CJumpIR &node) {
            std::string cond_reg = newAbstractRegister();

            generic_tile = Tile({
                tile(node.getCondition(), cond_reg),
                Assembly::Test(cond_reg, cond_reg),
                Assembly::JumpIfNZ(node.trueLabel())
            });
        },

        [&](JumpIR &node) {
            std::string target_reg = newAbstractRegister();

            generic_tile = Tile({
                tile(node.getTarget(), target_reg),
                Assembly::Jump(target_reg)
            });
        },

        [&](LabelIR &node) {
            generic_tile = Tile({Assembly::Label(node.getName())});
        },

        [&](MoveIR &node) {
            // Behaviour depends on target type
            std::visit(util::overload {

                [&](TempIR& target) {
                    generic_tile = Tile({
                        tile(node.getSource(), escapeTemporary(target.getName()))
                    });
                },

                [&](MemIR& target) {
                    std::string address_reg = newAbstractRegister();
                    std::string source_reg = newAbstractRegister();

                    generic_tile = Tile({
                        tile(node.getSource(), source_reg),
                        tile(target.getAddress(), address_reg),
                        Assembly::Mov(Assembly::MakeAddress(address_reg), source_reg)
                    });
                },

                [&](auto&) { THROW_CompilerError("Invalid MoveIR target"); }

            }, node.getTarget());
        },

        [&](ReturnIR &node) {
            if (current_is_entrypoint) {
                // Return a value by executing exit() system call with value in REG32_BASE
                if (!node.getRet()) THROW_CompilerError("invalid void return in entrypoint function");
                generic_tile.add_instructions_after({
                    Assembly::Comment("calculating return value and storing it in REG32_BASE"),
                    tile(*node.getRet(), Assembly::REG32_BASE),
                    Assembly::Comment("exit() system call"),
                    Assembly::Mov(Assembly::REG32_ACCUM, 1),
                    Assembly::SysCall()
                });
            } else {
                if (node.getRet()) {
                    // Return a value by placing it in REG32_ACCUM
                    generic_tile.add_instructions_after({
                        Assembly::Comment("return a value by placing it in REG32_ACCUM"),
                        tile(*node.getRet(), Assembly::REG32_ACCUM)
                    });
                }
                // Function epilogue
                generic_tile.add_instructions_after({
                    Assembly::Mov(Assembly::REG32_STACKPTR, Assembly::REG32_STACKBASEPTR),
                    Assembly::Pop(Assembly::REG32_STACKBASEPTR),
                    Assembly::Ret()
                });
            }
        },

        [&](CallIR &node) {
            // Push arguments onto stack, in reverse order (CDECL)
            for (auto &arg : node.getArgs()) {
                std::string argument_register = newAbstractRegister();
                generic_tile.add_instructions_before({
                    tile(*arg, argument_register),
                    Assembly::Push(argument_register)
                });
            }

            // Perform call instruction on function label
            if (auto name = std::get_if<NameIR>(&node.getTarget())) {
                generic_tile.add_instruction(Assembly::Call(name->getName()));
            } else {
                THROW_CompilerError("Function call target is not a label"); 
            }

            // Pop arguments from stack
            generic_tile.add_instruction(Assembly::Add(Assembly::REG32_STACKPTR, 4 * node.getNumArgs()));
        },

        [&](SeqIR &node) {
            generic_tile = Tile(std::vector<Instruction>());
            for (auto& stmt : node.getStmts()) {
                generic_tile.add_instruction(tile(*stmt));
            }
        },

        [&](ExpIR &node) {
            THROW_CompilerError("ExpIR should not exist after canonicalization"); 
        }
    }, ir);

    if (generic_tile.getCost() == Tile().getCost()) {
        THROW_CompilerError("Generic tile was not assigned to " + std::visit([&](auto &x){ return x.label(); }, ir));
    }
    decideIsCandidate(ir, generic_tile);

    return &statement_memo[&ir];
}
