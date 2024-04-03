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
                for (auto& body_instruction : body_tile->getFullInstructions()) {
                    output.push_back(body_instruction);
                }
            }
            return output;
        },
        [&](auto &node) -> std::list<std::string> { THROW_CompilerError("This should not happen"); }
    }, ir);
}

Tile* IRToTilesConverter::tile(ExpressionIR &ir, std::string &abstract_reg) {
    if (expression_memo.count(&ir)) {
        return &expression_memo[&ir].withAbstractRegister(abstract_reg);
    }

    std::visit(util::overload {
        [&](BinOpIR &node) {
            
            // Generic tile that is always applicable : store operands in abstract registers, then do something
            std::string operand1_reg = Assembly::newAbstractRegister();
            std::string operand2_reg = Assembly::newAbstractRegister();

            Tile generic_tile = Tile({
                tile(node.getLeft(), operand1_reg),
                tile(node.getRight(), operand2_reg)
            }, abstract_reg);

            switch (node.op) {
                case BinOpIR::OpType::ADD: {
                    generic_tile.add_instructions_after({
                        Assembly::Lea(abstract_reg, Assembly::MakeAddress(operand1_reg, operand2_reg))
                    });
                } 
                case BinOpIR::OpType::SUB: {
                    generic_tile.add_instructions_after({
                        Assembly::Lea(abstract_reg, Assembly::MakeAddress(operand1_reg, operand2_reg, -1))
                    });
                }
                case BinOpIR::OpType::MUL: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg),
                        Assembly::IMul(operand2_reg),
                        Assembly::Mov(abstract_reg, Assembly::REG32_ACCUM)
                    });
                }
                case BinOpIR::OpType::DIV: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg), // Move low 32 bits of divident into accumulator
                        Assembly::Xor(Assembly::REG32_DATA, Assembly::REG32_DATA), // Clear high 32 bits of dividend
                        Assembly::IDiv(operand2_reg),
                        Assembly::Mov(abstract_reg, Assembly::REG32_ACCUM) // Quotient stored in ACCUM
                    });
                }
                case BinOpIR::OpType::MOD: {
                    generic_tile.add_instructions_after({
                        Assembly::Mov(Assembly::REG32_ACCUM, operand1_reg), // Move low 32 bits of divident into accumulator
                        Assembly::Xor(Assembly::REG32_DATA, Assembly::REG32_DATA), // Clear high 32 bits of dividend
                        Assembly::IDiv(operand2_reg),
                        Assembly::Mov(abstract_reg, Assembly::REG32_DATA) // Remainder stored in DATA
                    });
                }
                case BinOpIR::OpType::AND: {
                    generic_tile.add_instructions_after({
                        Assembly::And(operand1_reg, operand2_reg),
                        Assembly::Mov(abstract_reg, operand1_reg)
                    });
                }
                case BinOpIR::OpType::OR: {
                    generic_tile.add_instructions_after({
                        Assembly::Or(operand1_reg, operand2_reg),
                        Assembly::Mov(abstract_reg, operand1_reg)
                    });
                }
                case BinOpIR::OpType::EQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetZ(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(abstract_reg, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::NEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetNZ(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(abstract_reg, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::LT: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetL(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(abstract_reg, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::GT: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetG(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(abstract_reg, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::LEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetLE(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(abstract_reg, Assembly::REG8L_ACCUM)
                    });
                }
                case BinOpIR::OpType::GEQ: {
                    generic_tile.add_instructions_after({
                        Assembly::Cmp(operand1_reg, operand2_reg),
                        Assembly::SetGE(Assembly::REG8L_ACCUM),
                        Assembly::MovZX(abstract_reg, Assembly::REG8L_ACCUM)
                    });
                }
                default: {
                    THROW_CompilerError("Operation is not supported in Joosc"); 
                }

                decideIsCandidate(ir, generic_tile);
            }
        },

        [&](ConstIR &node) {
            // 32-bit immediate
            Tile const_tile = Tile({
                Assembly::Mov(abstract_reg, std::to_string(node.getValue()))
            }, abstract_reg);

            decideIsCandidate(ir, const_tile);
        },

        // [&](MemIR &node) {},

        [&](NameIR &node) {
            Tile name_tile = Tile({
                Assembly::Mov(abstract_reg, node.getName())
            }, abstract_reg);

            decideIsCandidate(ir, name_tile);
        },

        [&](TempIR &node) {
            Tile temp_tile = Tile({
                Assembly::Mov(abstract_reg, node.getName())
            }, abstract_reg);

            decideIsCandidate(ir, temp_tile);
        },

        [&](ESeqIR &node) {
            THROW_CompilerError("ESeqIR should not exist after canonicalization"); 
        },

        [&](CallIR &node) {
            THROW_CompilerError("CallIR should not be considered an expression after canonicalization"); 
        },

        [&](auto &node) { THROW_CompilerError("Unimplemented"); }
    }, ir);

    return &expression_memo[&ir];
}

Tile* IRToTilesConverter::tile(StatementIR &ir) {
    if (statement_memo.count(&ir)) {
        return &statement_memo[&ir];
    }
    
    std::visit(util::overload {
        // [&](CJumpIR &node) {},

        [&](JumpIR &node) {
            std::string target_reg = Assembly::newAbstractRegister();

            Tile jump_tile = Tile({
                tile(node.getTarget(), target_reg),
                Assembly::Jump(target_reg)
            });

            decideIsCandidate(ir, jump_tile);
        },

        [&](LabelIR &node) {
            Tile label_tile = Tile({Assembly::Label(node.getName())});
            decideIsCandidate(ir, label_tile);
        },

        [&](MoveIR &node) {
            std::string target_reg = Assembly::newAbstractRegister();
            std::string source_reg = Assembly::newAbstractRegister();

            Tile move_tile = Tile({
                tile(node.getTarget(), target_reg),
                tile(node.getSource(), source_reg),
                Assembly::Mov(target_reg, source_reg)
            });

            decideIsCandidate(ir, move_tile);
        },

        // [&](ReturnIR &node) {},

        // [&](CallIR &node) {},

        [&](SeqIR &node) {
            Tile seq_tile;

            for (auto& stmt : node.getStmts()) {
                seq_tile.add_instruction(tile(*stmt));
            }

            decideIsCandidate(ir, seq_tile);
        },

        [&](ExpIR &node) {
            THROW_CompilerError("ExpIR should not exist after canonicalization"); 
        },

        [&](auto &node) { THROW_CompilerError("Unimplemented"); }
    }, ir);

    return &statement_memo[&ir];
}
