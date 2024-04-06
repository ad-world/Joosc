#include "ir-tiling.h"
#include "utillities/util.h"
#include "exceptions/exceptions.h"

#include "assembly.h"

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
                output.push_back(Assembly::Label(func->getName()));

                auto body_tile = tile(func->getBody());

                // RegisterAllocator::allocateRegisters(body_tile); (TODO)

                // Function prologue
                int temp_count = 0;
                output.push_back(Assembly::Push(Assembly::REG32_BP));
                output.push_back(Assembly::Mov(Assembly::REG32_BP, Assembly::REG32_SP));
                output.push_back(Assembly::Sub(Assembly::REG32_SP, 4*temp_count));

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

ExpressionTile IRToTilesConverter::tile(ExpressionIR &ir, std::string &abstract_reg) {
    if (expression_memo.count(&ir)) {
        return expression_memo[&ir].pairWith(abstract_reg);
    }

    // Generic tile that must be applicable, if no specialized tiles are applicable
    Tile generic_tile;

    std::visit(util::overload {
        [&](BinOpIR &node) {
            
            // Generic tile that is always applicable : store operands in abstract registers, then do something
            std::string operand1_reg = Assembly::newAbstractRegister();
            std::string operand2_reg = Assembly::newAbstractRegister();

            generic_tile = Tile({
                tile(node.getLeft(), operand1_reg),
                tile(node.getRight(), operand2_reg)
            });

            switch (node.op) {
                case BinOpIR::OpType::ADD: {
                    generic_tile.add_instructions_after({
                        Assembly::Lea(Tile::ABSTRACT_REG, Assembly::MakeAddress(operand1_reg, operand2_reg))
                    });
                } 
                case BinOpIR::OpType::SUB: {
                    generic_tile.add_instructions_after({
                        Assembly::Lea(Tile::ABSTRACT_REG, Assembly::MakeAddress(operand1_reg, operand2_reg, -1))
                    });
                }
                case BinOpIR::OpType::MUL: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg),
                        Assembly::IMul(operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, Assembly::REG32_ACCUM)
                    });
                }
                case BinOpIR::OpType::DIV: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg), // Move low 32 bits of divident into accumulator
                        Assembly::Xor(Assembly::REG32_DATA, Assembly::REG32_DATA), // Clear high 32 bits of dividend
                        Assembly::IDiv(operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, Assembly::REG32_ACCUM) // Quotient stored in ACCUM
                    });
                }
                case BinOpIR::OpType::MOD: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg), // Move low 32 bits of divident into accumulator
                        Assembly::Xor(Assembly::REG32_DATA, Assembly::REG32_DATA), // Clear high 32 bits of dividend
                        Assembly::IDiv(operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, Assembly::REG32_DATA) // Remainder stored in DATA
                    });
                }
                case BinOpIR::OpType::AND: {
                    generic_tile.add_instructions_after({
                        Assembly::And(operand1_reg, operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, operand1_reg)
                    });
                }
                case BinOpIR::OpType::OR: {
                    generic_tile.add_instructions_after({
                        Assembly::Or(operand1_reg, operand2_reg),
                        Assembly::Mov(Tile::ABSTRACT_REG, operand1_reg)
                    });
                }
                case BinOpIR::OpType::EQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetZ(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::NEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetNZ(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::LT: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetL(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::GT: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetG(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::LEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetLE(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::GEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetGE(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(Tile::ABSTRACT_REG, Assembly::REG8L_ACCUM)
                    });
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
            std::string address_reg = Assembly::newAbstractRegister();

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
            generic_tile = Tile({
                Assembly::Mov(Tile::ABSTRACT_REG, node.getName())
            });
        },

        [&](ESeqIR &node) {
            THROW_CompilerError("ESeqIR should not exist after canonicalization"); 
        },

        [&](CallIR &node) {
            THROW_CompilerError("CallIR should not be considered an expression after canonicalization"); 
        },

        [&](auto &node) { THROW_CompilerError("Unimplemented"); }
    }, ir);

    if (generic_tile.getCost() == Tile().getCost()) THROW_CompilerError("Generic tile was not assigned");
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
            std::string cond_reg = Assembly::newAbstractRegister();

            generic_tile = Tile({
                tile(node.getCondition(), cond_reg),
                Assembly::Test(cond_reg, cond_reg),
                Assembly::JumpIfNZ(node.trueLabel())
            });
        },

        [&](JumpIR &node) {
            std::string target_reg = Assembly::newAbstractRegister();

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
                    std::string source_reg = Assembly::newAbstractRegister();

                    generic_tile = Tile({
                        tile(node.getSource(), source_reg),
                        Assembly::Mov(target.getName(), source_reg)
                    });
                },

                [&](MemIR& target) {
                    std::string address_reg = Assembly::newAbstractRegister();
                    std::string source_reg = Assembly::newAbstractRegister();

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
            if (node.getRet()) {
                // Return a value by placing it in REG32_ACCUM
                std::string value_reg = Assembly::newAbstractRegister();
                generic_tile = Tile({
                    tile(*node.getRet(), value_reg),
                    Assembly::Mov(Assembly::REG32_ACCUM, value_reg)
                });
            }
            // Function epilogue
            generic_tile.add_instructions_after({
                Assembly::Mov(Assembly::REG32_SP, Assembly::REG32_BP),
                Assembly::Pop(Assembly::REG32_BP),
                Assembly::Ret()
            });
        },

        [&](CallIR &node) {
            // Push arguments onto stack
            for (auto &arg : node.getArgs()) {
                std::string argument_register = Assembly::newAbstractRegister();
                generic_tile.add_instructions_after({
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
            generic_tile.add_instruction(Assembly::Add(Assembly::REG32_SP, 4 * node.getNumArgs()));
        },

        [&](SeqIR &node) {
            for (auto& stmt : node.getStmts()) {
                generic_tile.add_instruction(tile(*stmt));
            }
        },

        [&](ExpIR &node) {
            THROW_CompilerError("ExpIR should not exist after canonicalization"); 
        },

        [&](auto &node) { THROW_CompilerError("Unimplemented"); }
    }, ir);

    if (generic_tile.getCost() == Tile().getCost()) THROW_CompilerError("Generic tile was not assigned");
    decideIsCandidate(ir, generic_tile);

    return &statement_memo[&ir];
}
