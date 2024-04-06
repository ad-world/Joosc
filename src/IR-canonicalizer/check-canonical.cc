#include "check-canonical.h"
#include "exceptions/exceptions.h"
#include "utillities/overload.h"
#include <variant>
#include <iostream>

void CanonicalChecker::notCanonicalBecause(const std::string& reason) {
    if (output_reason) {
        std::cerr << reason << "\n";
    }
    result = false;
}

bool CanonicalChecker::check(IR &ir, bool output_reason) {
    this->output_reason = output_reason;

    std::visit(util::overload {
        [&](CompUnitIR &node) {
            for (auto& func : node.getFunctionList()) {
                if (auto body = std::get_if<SeqIR>(&func->getBody())) {
                    current_body = &body->getStmts();
                    current_statement = 0;

                    for (auto& stmt : body->getStmts()) {
                        check(*stmt);
                        ++current_statement;
                    }
                } else {
                    notCanonicalBecause("Function body is not SeqIR");
                }
            }
        },
        [&](auto &node) { THROW_CompilerError("This should not happen"); }
    }, ir);

    return result;
}

void CanonicalChecker::check(ExpressionIR &ir) {
    return std::visit(util::overload {
        [&](BinOpIR &node) {
            check(node.getLeft());
            check(node.getRight());
        },

        [&](CallIR &node) {
            notCanonicalBecause("CallIR should not be considered an expression in canonical form");
        },

        [&](ConstIR &node) {},

        [&](ESeqIR &node) {
            notCanonicalBecause("ESeqIR should not exist in canonical form");
        },

        [&](MemIR &node) {
            check(node.getAddress());
        },

        [&](NameIR &node) {},

        [&](TempIR &node) {}
    }, ir);
}

void CanonicalChecker::check(StatementIR &ir) {
    return std::visit(util::overload {
        [&](CJumpIR &node) {
            // False label should be fall-through
            auto cJumpFalseLabel = node.falseLabel();

            if (current_statement + 1 == current_body->size()) {
                notCanonicalBecause("No statement after CJumpIR");
            } else {
                auto& next_stmt = current_body->at(current_statement+1);
                if (auto next_stmt_as_label = std::get_if<LabelIR>(next_stmt.get())) {
                    if (next_stmt_as_label->getName() != cJumpFalseLabel)
                        notCanonicalBecause("Statement after CJumpIR is not false label");
                } else {
                    notCanonicalBecause("Statement after CJumpIR is not a label");
                }
            }
        },

        [&](ExpIR &node) {
            notCanonicalBecause("ExpIR should not exist in canonical form");
        },

        [&](JumpIR &node) {
            check(node.getTarget());
        },

        [&](LabelIR &node) {},

        [&](MoveIR &node) {
            check(node.getTarget());
            check(node.getSource());
        },

        [&](ReturnIR &node) {
            if (node.getRet()) check(*node.getRet());
        },

        [&](SeqIR &node) {
            notCanonicalBecause("Non top-level SeqIR should not exist in canonical form");
        },

        [&](CallIR &node) {
            check(node.getTarget());
            for (auto& arg : node.getArgs()) {
                check(*arg);
            }
        }
    }, ir);
}
