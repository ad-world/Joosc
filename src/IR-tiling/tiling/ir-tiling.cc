#include "ir-tiling.h"
#include "utillities/util.h"
#include "exceptions/exceptions.h"
#include "IR-tiling/assembly/assembly.h"
#include "IR-tiling/register-allocation/register-allocator.h"
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

ExpressionTile IRToTilesConverter::tile(const std::string &abstract_reg, ExpressionIR &ir) {
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
                tile(operand1_reg, node.getLeft()),
                tile(operand2_reg, node.getRight())
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
                        Assembly::Cmp(operand2_reg, "0"), // Check for division by zero
                        Assembly::Je("__exception"), // Jump to exception handler if division by zero
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg), // Move low 32 bits of dividend into accumulator
                        Assembly::Cdq(), // Sign extend REG32_ACCUM into REG32_DATA
                        Assembly::IDiv(operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, Assembly::REG32_ACCUM) // Quotient stored in ACCUM
                    });
                    break;
                }
                case BinOpIR::OpType::MOD: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand2_reg, "0"), // Check for division by zero
                        Assembly::Je("__exception"), // Jump to exception handler if division by zero
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg), // Move low 32 bits of dividend into accumulator
                        Assembly::Cdq(), // Sign extend REG32_ACCUM into REG32_DATA
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
                tile(address_reg, node.getAddress()),
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
                        Assembly::MakeAddress(Assembly::REG32_STACKBASEPTR, "", 1, 4 * (arg_num + 2))
                    )
                });
            } 
            // Special temp : Static variable in .data section
            else if (node.isGlobal) {
                generic_tile = Tile({
                    Assembly::Mov(
                        Tile::ABSTRACT_REG,
                        Assembly::MakeAddress(node.getName())
                    )
                });
            }
            // Not special
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
                tile(cond_reg, node.getCondition()),
                Assembly::Test(cond_reg, cond_reg),
                Assembly::JumpIfNZ(node.trueLabel())
            });
        },

        [&](JumpIR &node) {
            std::string target_reg = newAbstractRegister();

            generic_tile = Tile({
                tile(target_reg, node.getTarget()),
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
                    if (target.isGlobal) {
                        // Update global variable
                        std::string temp_value_reg = newAbstractRegister();

                        generic_tile = Tile({
                            tile(temp_value_reg, node.getSource()),
                            Assembly::Mov(Assembly::MakeAddress(target.getName()), temp_value_reg)
                        });
                    } else {
                        generic_tile = Tile({
                            tile(escapeTemporary(target.getName()), node.getSource())
                        });
                    }
                },

                [&](MemIR& target) {
                    std::string address_reg = newAbstractRegister();
                    std::string source_reg = newAbstractRegister();

                    generic_tile = Tile({
                        tile(source_reg, node.getSource()),
                        tile(address_reg, target.getAddress()),
                        Assembly::Mov(Assembly::MakeAddress(address_reg), source_reg)
                    });
                },

                [&](auto&) { THROW_CompilerError("Invalid MoveIR target"); }

            }, node.getTarget());
        },

        [&](ReturnIR &node) {
            if (node.getRet()) {
                // Return a value by placing it in REG32_ACCUM
                generic_tile.add_instructions_after({
                    tile(Assembly::REG32_ACCUM, *node.getRet())
                });
            }
            // Function epilogue
            generic_tile.add_instructions_after({
                Assembly::Comment("function epilogue"),
                Assembly::Mov(Assembly::REG32_STACKPTR, Assembly::REG32_STACKBASEPTR),
                Assembly::Pop(Assembly::REG32_STACKBASEPTR),
                Assembly::Ret()
            });
        },

        [&](CallIR &node) {
            // Push arguments onto stack, in reverse order (CDECL)
            for (auto &arg : node.getArgs()) {
                std::string argument_register = newAbstractRegister();
                generic_tile.add_instructions_after({
                    tile(argument_register, *arg),
                    Assembly::Push(argument_register)
                });
            }
            generic_tile.add_instructions_before({Assembly::Comment("Call: pushing arguments onto stack")});

            // Perform call instruction on function label
            if (auto name = std::get_if<NameIR>(&node.getTarget())) {
                generic_tile.add_instruction(Assembly::Call(name->getName()));
            } else {
                THROW_CompilerError("Function call target is not a label"); 
            }

            // Pop arguments from stack
            generic_tile.add_instruction(Assembly::Comment("Call: popping arguments off stack"));
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
        },

        [&](CommentIR &node) {
            generic_tile = Tile(std::vector<Instruction>());
        }
    }, ir);

    if (generic_tile.getCost() == Tile().getCost()) {
        THROW_CompilerError("Generic tile was not assigned to " + std::visit([&](auto &x){ return x.label(); }, ir));
    }
    decideIsCandidate(ir, generic_tile);

    return &statement_memo[&ir];
}
