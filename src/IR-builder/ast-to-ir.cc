#include "ast-to-ir.h"
#include "IR/ir_variant.h"
#include "type-checker/typechecker.h"
#include "utillities/overload.h"
#include "utillities/util.h"
#include "variant-ast/expressions.h"
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include "exceptions/exceptions.h"
#include "IR-interpreter/simulation/simulation.h"
#include "IR/code-gen-constants.h"
using namespace std;

/***************************************************************
                            EXPRESSIONS
 ***************************************************************/

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(Expression &expr) {
    auto expr_ir = std::visit(util::overload{
        [&](auto &node) { return convert(node); }
    }, expr);
    return expr_ir;
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(Assignment &expr) {
    assert(expr.assigned_to);
    assert(expr.assigned_from);

    auto dest = convert(*expr.assigned_to);
    auto src = convert(*expr.assigned_from);
    vector<unique_ptr<StatementIR>> seq_vec;

    auto convert_eseq_ir = [&](unique_ptr<ExpressionIR> &dest) {
        // Make sure dest is either Temp or MemIR
        std::visit(util::overload{
            [&](TempIR &temp) { /* do nothing */ },
            [&](MemIR &mem) { /* do nothing */ },
            [&](ESeqIR &eseq) {
                // Replace dest with expression from eseq
                std::visit(util::overload{
                    [&](SeqIR &seq) {
                        seq_vec.clear();
                        for ( auto &stmt : seq.getStmts() ) {
                            seq_vec.push_back(std::move(stmt));
                        }
                    },
                    [&](auto &node) { THROW_CompilerError("ESEQ is not composed of a sequence IR"); }
                }, eseq.getStmt());
                dest = make_unique<ExpressionIR>(std::move(eseq.getExpr()));
            },
            [&](auto &node) { THROW_CompilerError("Assignment destination is not TempIR or MemIR"); }
        }, *dest);
    };

    convert_eseq_ir(dest);

    auto src_temp = TempIR::generateName("assign_src");
    seq_vec.push_back(MoveIR::makeStmt(
        TempIR::makeExpr(src_temp),
        std::move(src)
    ));
    seq_vec.push_back(MoveIR::makeStmt(
        std::move(dest),
        TempIR::makeExpr(src_temp)
    ));

    auto statement_ir = SeqIR::makeStmt(std::move(seq_vec));
    auto expression_ir = TempIR::makeExpr(src_temp);
    auto eseq = ESeqIR::makeExpr(std::move(statement_ir), std::move(expression_ir));

    return eseq;
}

std::unique_ptr<ExpressionIR> handleShortCircuit(BinOpIR::OpType op, std::unique_ptr<ExpressionIR> e1, std::unique_ptr<ExpressionIR> e2) {
    switch (op) {
        case BinOpIR::AND: {
            auto temp_name = TempIR::generateName("t_and");
            vector<unique_ptr<StatementIR>> seq_vec;
            // Move(t_and, e1)
            seq_vec.push_back(
                MoveIR::makeStmt(
                    TempIR::makeExpr(temp_name),
                    std::move(e1)
                )
            );
            // CJump(e1 == 0, false, true)
            auto true_name = LabelIR::generateName("sc_true");
            auto false_name = LabelIR::generateName("sc_false");
            seq_vec.push_back(
                CJumpIR::makeStmt(
                    BinOpIR::makeNegate(TempIR::makeExpr(temp_name)),
                    false_name,
                    true_name
                )
            );
            // sc_true:
            seq_vec.push_back(
                LabelIR::makeStmt(true_name)
            );
            // Evaluate e2
            // Move(t_and, e2)
            seq_vec.push_back(
                MoveIR::makeStmt(
                    TempIR::makeExpr(temp_name),
                    std::move(e2)
                )
            );
            // sc_false:
            seq_vec.push_back(
                LabelIR::makeStmt(false_name)
            );
            // ESEQ({...}, t_and)
            return ESeqIR::makeExpr(
                SeqIR::makeStmt(std::move(seq_vec)),
                TempIR::makeExpr(temp_name)
            );
        }
        case BinOpIR::OR: {
            auto temp_name = TempIR::generateName("t_or");
            vector<unique_ptr<StatementIR>> seq_vec;
            // Move(t_or, e1)
            seq_vec.push_back(
                MoveIR::makeStmt(
                    TempIR::makeExpr(temp_name),
                    std::move(e1)
                )
            );
            // CJump(t_or, false, true)
            auto true_name = LabelIR::generateName("sc_true");
            auto false_name = LabelIR::generateName("sc_false");
            seq_vec.push_back(
                CJumpIR::makeStmt(
                    TempIR::makeExpr(temp_name),
                    true_name,
                    false_name
                )
            );
            // sc_false:
            seq_vec.push_back(
                LabelIR::makeStmt(false_name)
            );
            // Evaluate e2
            // Move(t_or, e2)
            seq_vec.push_back(
                MoveIR::makeStmt(
                    TempIR::makeExpr(temp_name),
                    std::move(e2)
                )
            );
            // sc_true:
            seq_vec.push_back(
                LabelIR::makeStmt(true_name)
            );
            // ESEQ({...}, t_and)
            return ESeqIR::makeExpr(
                SeqIR::makeStmt(std::move(seq_vec)),
                TempIR::makeExpr(temp_name)
            );
        }
        default: break;
    }
    THROW_ASTtoIRError("Invalid operator for short circuiting");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(InfixExpression &expr) {
    auto left = convert(*expr.expression1);
    auto right = convert(*expr.expression2);
    BinOpIR::OpType bin_op;

    auto make_expr = [&](BinOpIR::OpType op) {
        return BinOpIR::makeExpr(op, std::move(left), std::move(right));
    };

    switch ( expr.op ) {
        case InfixOperator::BOOLEAN_OR:
            return handleShortCircuit(BinOpIR::OR, std::move(left), std::move(right));
        case InfixOperator::EAGER_OR:
            return make_expr(BinOpIR::OR);
        case InfixOperator::BOOLEAN_AND:
            return handleShortCircuit(BinOpIR::AND, std::move(left), std::move(right));
        case InfixOperator::EAGER_AND:
            return make_expr(BinOpIR::AND);
        case InfixOperator::BOOLEAN_EQUAL:
            return make_expr(BinOpIR::EQ);
        case InfixOperator::BOOLEAN_NOT_EQUAL:
            return make_expr(BinOpIR::NEQ);
        case InfixOperator::PLUS:
            return make_expr(BinOpIR::ADD);
        case InfixOperator::MINUS:
            return make_expr(BinOpIR::SUB);
        case InfixOperator::DIVIDE:
            return make_expr(BinOpIR::DIV);
        case InfixOperator::MULTIPLY:
            return make_expr(BinOpIR::MUL);
        case InfixOperator::MODULO:
            return make_expr(BinOpIR::MOD);
        case InfixOperator::LESS_THAN:
            return make_expr(BinOpIR::LT);
        case InfixOperator::GREATER_THAN:
            return make_expr(BinOpIR::GT);
        case InfixOperator::LESS_THAN_EQUAL:
            return make_expr(BinOpIR::LEQ);
        case InfixOperator::GREATER_THAN_EQUAL:
            return make_expr(BinOpIR::GEQ);
    }

    THROW_ASTtoIRError("Unhanlded InfixOperator");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(PrefixExpression &expr) {
    assert(expr.expression);

    auto expression = convert(*expr.expression);
    switch ( expr.op ) {
        case PrefixOperator::MINUS: {
            auto zero = ConstIR::makeZero();
            auto expression_ir = std::make_unique<ExpressionIR>(
                std::in_place_type<BinOpIR>, BinOpIR::SUB, std::move(zero), std::move(expression)
            );
            return expression_ir;
        }
        case PrefixOperator::NEGATE: {
            #warning This can be handled by embedding booleans into the jump code (L16)
            return BinOpIR::makeNegate(std::move(expression));
        }
    }

    THROW_ASTtoIRError("Unhanled PrefixOperator");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(CastExpression &expr) {
    assert(expr.expression);

    const int8_t byte_cast = 0xFF;
    const int16_t short_cast = 0xFFFF;
    const int32_t int_cast = 0xFFFFFFFF;
    const uint16_t char_cast = 0xFFFF;
    auto converted_expr = convert(*expr.expression);

    return std::visit(util::overload{
        [&](Literal &lit) {
            if ( auto primitive = expr.type->link.getIfIsPrimitive() ) {
                unique_ptr<ExpressionIR> expr;

                std::visit(util::overload{
                    [&](int64_t int_lit) {
                        switch (*primitive) {
                            case PrimitiveType::BYTE:
                                expr = ConstIR::makeExpr(byte_cast & (int8_t) int_lit);
                                break;
                            case PrimitiveType::SHORT:
                                expr = ConstIR::makeExpr(short_cast & (int16_t) int_lit);
                                break;
                            case PrimitiveType::INT:
                                expr = ConstIR::makeExpr(int_lit);
                                break;
                            case PrimitiveType::CHAR:
                                expr = ConstIR::makeExpr(char_cast & (uint16_t) int_lit);
                                break;
                            default:
                                THROW_ASTtoIRError("Error casting to type int");
                        }
                    },
                    [&](char char_lit) {
                        switch (*primitive) {
                            case PrimitiveType::BYTE: // fallthru
                                expr = ConstIR::makeExpr(byte_cast & (int8_t) char_lit);    // TODO: test
                            case PrimitiveType::SHORT: // fallthru
                                expr = ConstIR::makeExpr(short_cast & (int16_t) char_lit);  // TODO: test
                            case PrimitiveType::INT: // fallthru
                            case PrimitiveType::CHAR:
                                // Widening
                                expr = ConstIR::makeExpr(char_lit);
                                break;
                            default:
                                THROW_ASTtoIRError("Error casting to type char");
                        }
                    },
                    [&](string str_lit) {
                        THROW_ASTtoIRError("Error casting a string to a primitive type");
                    },
                    [&](bool bool_lit) {
                        if ( *primitive == PrimitiveType::BOOLEAN ) {
                            expr = bool_lit ? ConstIR::makeOne() : ConstIR::makeZero();
                            return;
                        }
                        THROW_ASTtoIRError("Error casting to type boolean");
                    },
                    [&](nullptr_t null_lit) {
                        if ( *primitive == PrimitiveType::NULL_T ) {
                            expr = ConstIR::makeZero();
                            return;
                        }
                        THROW_ASTtoIRError("Error casting to type NULL");
                    },
                }, lit);

                return expr;
            } else {
                THROW_ASTtoIRError("TODO: Deferred to A6 - non-primitive casts");
            } // if
        },
        [&](auto &node) { return convert(*expr.expression); }
    }, *expr.expression);
}

// int64_t, bool, char, std::string, std::nullptr_t
std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(Literal &expr) {
    if ( auto lit = std::get_if<int64_t>(&expr) ) {
        // int64_t
        int64_t value = *lit;
        auto const_ir = make_unique<ExpressionIR>(
            in_place_type<ConstIR>,
            value
        );
        return const_ir;
    } else if ( auto lit = std::get_if<bool>(&expr) ) {
        // bool
        bool value = *lit;
        auto const_ir = (value)
            ? (ConstIR::makeOne())
            : (ConstIR::makeZero());
        return const_ir;
    } else if ( auto lit = std::get_if<char>(&expr) ) {
        // char
        char value = *lit;
        auto const_ir = make_unique<ExpressionIR>(
            in_place_type<ConstIR>,
            value
        );
        return const_ir;
    } else if ( auto lit = std::get_if<string>(&expr) ) {
        // string
        string value = *lit;

        THROW_ASTtoIRError("TODO: Deferred to A6 - unhandled literal type");

    } else if ( auto lit = std::get_if<nullptr_t>(&expr) ) {
        // nullptr_t
        return ConstIR::makeZero();
    } else {
        THROW_ASTtoIRError("Unhandled literal type");
    }
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ClassInstanceCreationExpression &expr) {
    assert(expr.class_name);

    // Call the appropriate constructor of the class
    THROW_ASTtoIRError("TODO: Deferred to A6 - feature of OOP");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(FieldAccess &expr) {
    assert(expr.expression);
    assert(expr.identifier);

    auto link = TypeChecker::getLink(*expr.expression);
    if ( auto array = link.is_array && expr.identifier->name == "length" ) {
        // Array length
        return MemIR::makeExpr(
            BinOpIR::makeExpr(
                BinOpIR::SUB,
                convert(*expr.expression),
                ConstIR::makeWords()
            )
        );
    } else if ( auto linkedClass = link.getIfNonArrayIsClass() ) {
        std::string varName = expr.identifier->name;
        std::string name = linkedClass->full_qualified_name + "." + varName;

        return TempIR::makeExpr(name);
    }
    THROW_ASTtoIRError("TODO: Deferred to A6 - non-static field");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(MethodInvocation &expr) {
    auto called_method = expr.called_method;

    if (called_method->ast_reference->hasModifier(Modifier::STATIC) || true) {
        // Args vectors
        vector<unique_ptr<ExpressionIR>> call_args_vec = {};
        for ( auto &arg : expr.arguments ) {
            call_args_vec.push_back(std::move(convert(arg)));
        }

        return CallIR::makeExpr(
            NameIR::makeExpr(CGConstants::uniqueMethodLabel(expr.called_method)),
            std::move(call_args_vec)
        );
    } else {
        // Non-static methods
        THROW_ASTtoIRError("TODO: Deferred to A6 - non-static method");
    }
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ArrayAccess &expr) {
    assert(expr.array);
    assert(expr.selector);

    // Get array in temp
    string array_name = TempIR::generateName("array");
    auto get_array = MoveIR::makeStmt(
        TempIR::makeExpr(array_name),
        std::move(convert(*expr.array))
    );

    // Non-null check
    string error_name = LabelIR::generateName("error");
    string non_null_name = LabelIR::generateName("nonnull");
    auto non_null_check = CJumpIR::makeStmt(
        // NEQ(t_a, 0)
        BinOpIR::makeExpr(
            BinOpIR::NEQ,
            TempIR::makeExpr(array_name),
            ConstIR::makeZero()
        ),
        non_null_name,
        error_name
    );

    // Exception on null
    auto error_label = LabelIR::makeStmt(error_name);
    auto exception_call = ExpIR::makeStmt(std::move(CallIR::makeException()));

    // Non-null
    auto non_null_label = LabelIR::makeStmt(non_null_name);
    string selector_name = TempIR::generateName("selector");
    auto get_selector = MoveIR::makeStmt(
        TempIR::makeExpr(selector_name),
        std::move(convert(*expr.selector))
    );

    // Bounds check
    auto inbound_name = LabelIR::generateName("inbounds");
    auto bounds_check = CJumpIR::makeStmt(
        // t_i < 0 || t_i >= mem(t_a - 4)
        BinOpIR::makeExpr(
            BinOpIR::OR,
            // LT(t_i, 0)
            BinOpIR::makeExpr(
                BinOpIR::LT,
                TempIR::makeExpr(selector_name),
                ConstIR::makeZero()
            ),
            // GEQ(t_i, MEM(t_a - 4))
            BinOpIR::makeExpr(
                BinOpIR::GEQ,
                TempIR::makeExpr(selector_name),
                // MEM(t_a - 4)
                MemIR::makeExpr(
                    // SUB(t_a, 4)
                    BinOpIR::makeExpr(
                        BinOpIR::SUB,
                        TempIR::makeExpr(array_name),
                        ConstIR::makeWords()
                    )
                )
            )
        ),
        error_name,    // out of bounds
        inbound_name   // in bounds
    );

    // Inbound call
    auto inbound_label = LabelIR::makeStmt(inbound_name);
    auto inbound_call = MemIR::makeExpr(
        // t_a + 4 + (4 * t_i)
        BinOpIR::makeExpr(
            BinOpIR::ADD,
            TempIR::makeExpr(array_name),
            BinOpIR::makeExpr(
                BinOpIR::ADD,
                ConstIR::makeWords(),
                BinOpIR::makeExpr(
                    BinOpIR::MUL,
                    ConstIR::makeWords(),
                    TempIR::makeExpr(selector_name)
                )
            )
        )
    );

    vector<unique_ptr<StatementIR>> seq_vec;

    seq_vec.push_back(std::move(get_array));            // Temp t_a
    seq_vec.push_back(std::move(non_null_check));       // CJump(NEQ(t_a, 0), non_null, error)
    seq_vec.push_back(std::move(error_label));          // error:
    seq_vec.push_back(std::move(exception_call));       // CALL(NAME(__exception))
    seq_vec.push_back(std::move(non_null_label));       // non_null:
    seq_vec.push_back(std::move(get_selector));         // Temp t_i
    seq_vec.push_back(std::move(bounds_check));         // CJump(GEQ(t_i, MEM(t_a - 4)), error, inbound)
    seq_vec.push_back(std::move(inbound_label));        // inbound:

    auto eseq_ir = ESeqIR::makeExpr(
        SeqIR::makeStmt(std::move(seq_vec)),
        // ConstIR::makeZero()
        std::move(inbound_call)                         // MEM(t_a + 4 + 4*t_i)
    );

    return eseq_ir;
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(QualifiedThis &expr) {
    THROW_ASTtoIRError("QualifiedThis is not supported (I think)");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ArrayCreationExpression &expr) {
    assert(expr.type);
    assert(expr.expression);

    if ( auto primitive = expr.type->link.getIfIsPrimitive() ) {
        // Primitive type

        // Get inner expression
        auto size_name = TempIR::generateName("size");
        auto size_get = MoveIR::makeStmt(
            TempIR::makeExpr(size_name),
            convert(*expr.expression)
        );

        // Check non-negative
        auto error_name = LabelIR::generateName("error");
        auto non_negative_name = LabelIR::generateName("nonneg");
        auto non_negative_check = CJumpIR::makeStmt(
            // t_e >= 0
            BinOpIR::makeExpr(
                BinOpIR::GEQ,
                TempIR::makeExpr(size_name),
                ConstIR::makeZero()
            ),
            non_negative_name,
            error_name
        );

        // Error call
        auto error_label = LabelIR::makeStmt(error_name);
        auto error_call = ExpIR::makeStmt(std::move(CallIR::makeException()));

        // Allocate space
        auto non_negative_label = LabelIR::makeStmt(non_negative_name);
        auto array_name = TempIR::generateName("array");
        auto malloc = MoveIR::makeStmt(
            TempIR::makeExpr(array_name),
            CallIR::makeMalloc(
                // 4*t_size + 8
                BinOpIR::makeExpr(
                    BinOpIR::ADD,
                    BinOpIR::makeExpr(
                        BinOpIR::MUL,
                        ConstIR::makeWords(),
                        TempIR::makeExpr(size_name)
                    ),
                    ConstIR::makeWords(2)
                )
            )
        );

        // Write size
        auto write_size = MoveIR::makeStmt(
            MemIR::makeExpr(TempIR::makeExpr(array_name)),
            TempIR::makeExpr(size_name)
        );

        // Zero initialize array (loop)
        auto iterator_name = TempIR::generateName("iter");
        auto start_loop = LabelIR::generateName("start_loop");
        auto exit_loop = LabelIR::generateName("exit_loop");
        auto dummy_name = LabelIR::generateName("dummy");
        vector<unique_ptr<StatementIR>> seq_vec;

        seq_vec.push_back(std::move(size_get));
        seq_vec.push_back(std::move(non_negative_check));
        seq_vec.push_back(std::move(error_label));
        seq_vec.push_back(std::move(error_call));
        seq_vec.push_back(std::move(non_negative_label));
        seq_vec.push_back(std::move(malloc));
        seq_vec.push_back(std::move(write_size));

        seq_vec.push_back(
            // Move(t_i, 0)
            MoveIR::makeStmt(
                TempIR::makeExpr(iterator_name),
                ConstIR::makeZero()
            )
        );
        seq_vec.push_back(
            // start_loop:
            LabelIR::makeStmt(start_loop)
        );
        seq_vec.push_back(
            // MOVE(t_i, t_i + 4)
            MoveIR::makeStmt(
                TempIR::makeExpr(iterator_name),
                BinOpIR::makeExpr(
                    BinOpIR::ADD,
                    TempIR::makeExpr(iterator_name),
                    ConstIR::makeWords()
                )
            )
        );
        seq_vec.push_back(
            // CJump(t_i > 4 * t_size, exit_loop, start_loop)
            CJumpIR::makeStmt(
                BinOpIR::makeExpr(
                    BinOpIR::GT,
                    TempIR::makeExpr(iterator_name),
                    BinOpIR::makeExpr(
                        BinOpIR::MUL,
                        TempIR::makeExpr(size_name),
                        ConstIR::makeWords()
                    )
                ),
                exit_loop,
                dummy_name
            )
        );
        seq_vec.push_back(
            // dummy:
            LabelIR::makeStmt(dummy_name)
        );
        seq_vec.push_back(
            // MOVE(MEM(t_i + t_a + 4), 0) 
            MoveIR::makeStmt(
                MemIR::makeExpr(
                    BinOpIR::makeExpr(
                        BinOpIR::ADD,
                        TempIR::makeExpr(iterator_name),
                        BinOpIR::makeExpr(
                            BinOpIR::ADD,
                            TempIR::makeExpr(array_name),
                            ConstIR::makeWords()
                        )
                    )
                ),
                ConstIR::makeZero()
            )
        );
        seq_vec.push_back(
            // Jump to start_loop
            JumpIR::makeStmt(
                NameIR::makeExpr(start_loop)
            )
        );
        seq_vec.push_back(
            // exit_loop:
            LabelIR::makeStmt(exit_loop)
        );

        //  INIT t_i
        // start_loop:
        //  t_i = t_i + 4
        //  CJump to exit if outbounds
        // dummy:
        //  a[t_i] = 0
        //  Jump to start
        // exit_loop:
        return ESeqIR::makeExpr(
            SeqIR::makeStmt(std::move(seq_vec)),
            // t_array + 4
            BinOpIR::makeExpr(
                BinOpIR::ADD,
                TempIR::makeExpr(array_name),
                ConstIR::makeWords()
            )
        );
    } else {
        // Non-primitive type
        THROW_ASTtoIRError("TODO: Deferred to A6 - non-primitive array creation");
    }
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(QualifiedIdentifier &expr) {
    if (expr.identifiers.size() == 1) {
        // Local variable access
        auto identifier = expr.identifiers.front();
        #warning Not always a local variable access
        return TempIR::makeExpr(identifier.name);
    } else if ( expr.is_array_length ) {
        // Some array's length prop
        auto qid = expr.getQualifiedIdentifierWithoutLast();

        // MEM(arr - 4)
        return MemIR::makeExpr(BinOpIR::makeExpr(
            BinOpIR::SUB,
            convert(qid),
            ConstIR::makeWords()
        ));
    } else {
        // Static field access (typically)

        auto prefix = expr.getQualifiedIdentifierWithoutLast().identifiers;
        // Find class path
        if ( auto classObj = Util::root_package->findClassDeclaration(prefix) ) {
            // Parent is class in root package
            std::string varName = expr.identifiers.back().name;
            std::string name = classObj->full_qualified_name + "." + varName;

            return TempIR::makeExpr(name);
        } else if ( auto classObj = Util::root_package->findPackageDeclaration("java.lang")->findClassDeclaration(prefix) ) {
            // Parent is class in java.lang
            std::string varName = expr.identifiers.back().name;
            std::string name = classObj->full_qualified_name + "." + varName;

            return TempIR::makeExpr(name);
        }

        string qualified_str = expr.getQualifiedName();
        #warning Not always a static field
        return TempIR::makeExpr(qualified_str);
    }
    // THROW_ASTtoIRError("TODO: Deferred to A6 - qualified identifiers");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(InstanceOfExpression &expr) {
    THROW_ASTtoIRError("TODO: Deferred to A6 - instanceof");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ParenthesizedExpression &expr) {
    assert(expr.expression);
    return convert(*expr.expression);
}

/***************************************************************
                            STATEMENTS
 ***************************************************************/

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(Statement &stmt) {
    return std::visit(util::overload{
        [&](auto &node) { return convert(node); }
    }, stmt);
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(IfThenStatement &stmt) {
    assert(stmt.if_clause);
    assert(stmt.then_clause);

    auto if_true = LabelIR::generateName("if_true");
    auto if_exit = LabelIR::generateName("if_exit");
   
    vector<unique_ptr<StatementIR>> seq_vec;
    // CJump(expr == 0, exit, true)
    seq_vec.push_back(
        CJumpIR::makeStmt(
            BinOpIR::makeNegate(
                convert(*stmt.if_clause)
            ),
            if_exit,
            if_true
        )
    );

    // if_true:
    seq_vec.push_back(
        LabelIR::makeStmt(if_true)
    );

    // then_clause
    seq_vec.push_back(
        convert(*stmt.then_clause)
    );

    // if_exit:
    seq_vec.push_back(
        LabelIR::makeStmt(if_exit)
    );

    return SeqIR::makeStmt(std::move(seq_vec));
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(IfThenElseStatement &stmt) {
    assert(stmt.if_clause);
    assert(stmt.then_clause);
    assert(stmt.else_clause);

    auto if_true = LabelIR::generateName("if_true");
    auto if_false = LabelIR::generateName("if_false");
    auto if_exit = LabelIR::generateName("if_exit");
   
    vector<unique_ptr<StatementIR>> seq_vec;
    // CJump(expr == 0, false, true)
    seq_vec.push_back(
        CJumpIR::makeStmt(
            BinOpIR::makeNegate(
                convert(*stmt.if_clause)
            ),
            if_false,
            if_true
        )
    );

    // if_true:
    seq_vec.push_back(
        LabelIR::makeStmt(if_true)
    );

    // then_clause
    seq_vec.push_back(
        convert(*stmt.then_clause)
    );

    // jump to exit
    seq_vec.push_back(
        JumpIR::makeStmt(NameIR::makeExpr(if_exit))
    );

    // if_false:
    seq_vec.push_back(
        LabelIR::makeStmt(if_false)
    );

    // else_clause
    seq_vec.push_back(
        convert(*stmt.else_clause)
    );

    // jump to exit (fall through)

    // if_exit:
    seq_vec.push_back(
        LabelIR::makeStmt(if_exit)
    );

    return SeqIR::makeStmt(std::move(seq_vec));
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(WhileStatement &stmt) {
    assert(stmt.condition_expression);
    assert(stmt.body_statement);

    vector<unique_ptr<StatementIR>> seq_vec;
    auto while_start = LabelIR::generateName("while_start");
    auto while_true = LabelIR::generateName("while_true");
    auto while_exit = LabelIR::generateName("while_exit");

    // while_start:
    seq_vec.push_back(
        LabelIR::makeStmt(while_start)
    );

    // CJump(expr == 0, exit, true)
    seq_vec.push_back(
        CJumpIR::makeStmt(
            BinOpIR::makeNegate(
                convert(*stmt.condition_expression)
            ),
            while_exit,
            while_true
        )
    );

    // while_true:
    seq_vec.push_back(
        LabelIR::makeStmt(while_true)
    );

    // Body stmt
    seq_vec.push_back(
        convert(*stmt.body_statement)
    );

    // Jump to while_start
    seq_vec.push_back(
        JumpIR::makeStmt(NameIR::makeExpr(while_start))
    );

    // while_exit:
    seq_vec.push_back(
        LabelIR::makeStmt(while_exit)
    );

    return SeqIR::makeStmt(std::move(seq_vec));
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ForStatement &stmt) {
    assert(stmt.condition_expression);
    assert(stmt.body_statement);

    vector<unique_ptr<StatementIR>> seq_vec;
    auto for_start = LabelIR::generateName("for_start");
    auto for_true = LabelIR::generateName("for_true");
    auto for_exit = LabelIR::generateName("for_exit");

    // Init statement
    if (stmt.init_statement) {
        seq_vec.push_back(
            convert(*stmt.init_statement)
        );
    }

    // for_start:
    seq_vec.push_back(
        LabelIR::makeStmt(for_start)
    );

    // CJump(cond == 0, exit, true)
    seq_vec.push_back(
        CJumpIR::makeStmt(
            BinOpIR::makeNegate(
                convert(*stmt.condition_expression)
            ),
            for_exit,
            for_true
        )
    );

    // for_true:
    seq_vec.push_back(
        LabelIR::makeStmt(for_true)
    );

    // Body
    seq_vec.push_back(
        convert(*stmt.body_statement)
    );

    // Update
    if (stmt.update_statement) {
        seq_vec.push_back(
            convert(*stmt.update_statement)
        );
    }

    // Jump to start
    seq_vec.push_back(
        JumpIR::makeStmt(NameIR::makeExpr(for_start))
    );

    // for_exit:
    seq_vec.push_back(
        LabelIR::makeStmt(for_exit)
    );

    return SeqIR::makeStmt(std::move(seq_vec));
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(Block &stmt) {
    vector<unique_ptr<StatementIR>> seq_vec;

    for (auto &stmt : stmt.statements) {
        seq_vec.push_back(
            convert(stmt)
        );
    }

    if (seq_vec.empty()) {
        return SeqIR::makeEmpty();
    // } else if ( seq_vec.size() == 1 ) {
    //     return std::move(seq_vec.back());
    } else {
        return SeqIR::makeStmt(std::move(seq_vec));
    }
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(EmptyStatement &stmt) {
    return SeqIR::makeEmpty();
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ExpressionStatement &stmt) {
    return std::visit(
        [&](auto &expr) {
            return ExpIR::makeStmt(convert(expr));
        }, stmt
    );
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ReturnStatement &stmt) {
    if (stmt.return_expression) {
        return ReturnIR::makeStmt(convert(*stmt.return_expression));
    } else {
        return ReturnIR::makeStmt(ConstIR::makeZero());
    }
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(LocalVariableDeclaration &stmt) {
    assert(stmt.variable_declarator);
    assert(stmt.variable_declarator->expression);

    vector<unique_ptr<StatementIR>> seq_vec;
    auto var_name = stmt.variable_declarator->variable_name->name;

    // Move(var, rhs)
    #warning This will overwrite a previous LVD (should have some kind of stack)
    return MoveIR::makeStmt(
        TempIR::makeExpr(var_name),
        convert(*stmt.variable_declarator->expression)
    );
}

/***************************************************************
                            OPERATORS
 ***************************************************************/

void IRBuilderVisitor::operator()(ClassDeclaration &node) {
    // CREATE CompUnit
    comp_unit = {node.environment->identifier};

    // Add fields
    assert(node.environment->package_contained_in);
    for ( auto &field : node.field_declarations ) {
        if ( field.hasModifier(Modifier::STATIC) ) {
            // Static field
            std::string name = field.environment->full_qualified_name;

            assert(field.variable_declarator);
            if ( field.variable_declarator->expression ) {
                // Non-null
                try {
                    comp_unit.appendField(name, convert(*field.variable_declarator->expression));
                } catch (...) {
                    // TODO: remove
                    // Skip if unable to convert initializer
                }
            } else {
                // Null
                comp_unit.appendField(name, ConstIR::makeZero());
            } // if
        } // if
    } // for

    // Add functions
    this->visit_children(node);
}

void IRBuilderVisitor::operator()(MethodDeclaration &node) {
    if ( !node.hasModifier(Modifier::STATIC) || !node.environment->return_type.isPrimitive() ) {
        // Skip non-static methods
        // If non-primitive return type or non-static method, add an empty function

        auto label = CGConstants::uniqueMethodLabel(node.environment);

        // Create func_decl
        auto func_decl = make_unique<FuncDeclIR>(
            label,
            SeqIR::makeEmpty(),
            (int) node.parameters.size()
        );

        // Add func_decl to comp_unit
        comp_unit.appendFunc(label, std::move(func_decl));
        return;
    }

    if (node.environment->is_constructor) {
        // Constructors are an Object-Oriented feature that will be handled in A6
        return; 
    }

    if (node.body) {
        // CREATE FuncDecl

        // Add load arguments to body statement
        auto body_stmt = convert(*node.body);
        std::visit(util::overload{
            [&](SeqIR &seq) {
                vector<unique_ptr<StatementIR>> load_args;

                // Add parameter moves
                int arg_num = 0;
                for ( auto &param : node.parameters ) {
                    auto param_name = param.parameter_name->name;
                    auto abstract_arg_name = CGConstants::ABSTRACT_ARG_PREFIX + to_string(arg_num++);
                    auto arg_temp = TempIR::makeExpr(abstract_arg_name);
                    auto param_temp = TempIR::makeExpr(param_name);
                    load_args.push_back(
                        MoveIR::makeStmt(
                            std::move(param_temp),
                            std::move(arg_temp)
                        )
                    );
                }

                // Move body statements
                for ( auto &body_stmt : seq.getStmts() ) {
                    load_args.push_back(
                        std::move(body_stmt)
                    );
                }

                // Add implicit return to void functions
                if ( node.environment->return_type.isVoid() ) {
                    load_args.push_back(ReturnIR::makeStmt(ConstIR::makeZero()));
                }

                body_stmt = SeqIR::makeStmt(std::move(load_args));
            },
            [&](auto &node) {
                THROW_CompilerError("Method body should always return a SeqIR");
            }
        }, *body_stmt);

        auto label = CGConstants::uniqueMethodLabel(node.environment);

        // Create func_decl
        auto func_decl = make_unique<FuncDeclIR>(
            label,
            std::move(body_stmt),
            (int) node.parameters.size()
        );

        // Add func_decl to comp_unit
        comp_unit.appendFunc(label, std::move(func_decl));
    }
}

// Rest of the operators are probably not needed?
