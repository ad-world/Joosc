#include "ast-to-ir.h"
#include "IR/ir.h"
#include "IR/ir_variant.h"
#include "utillities/overload.h"
#include "variant-ast/expressions.h"
#include <memory>
#include <utility>
#include <variant>
#include "exceptions/exceptions.h"
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
    assert(expr.assigned_to.get());
    assert(expr.assigned_from.get());

    auto dest = convert(*expr.assigned_to);
    auto dest_address = convert(*expr.assigned_to);
    auto src = convert(*expr.assigned_from);

    // TODO: see if this can be simplified (instead of getting dest twice, use temp?)
    auto statement_ir = std::make_unique<StatementIR>(std::in_place_type<MoveIR>, std::move(dest), std::move(src));
    auto expression_ir = std::make_unique<ExpressionIR>(std::in_place_type<MemIR>, std::move(dest_address));
    auto eseq = std::make_unique<ExpressionIR>(std::in_place_type<ESeqIR>, std::move(statement_ir), std::move(expression_ir));

    return eseq;
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(InfixExpression &expr) {
    auto left = convert(*expr.expression1);
    auto right = convert(*expr.expression2);
    BinOpIR::OpType bin_op;

    switch ( expr.op ) {
        case InfixOperator::BOOLEAN_OR:
        case InfixOperator::EAGER_OR:
            bin_op = BinOpIR::OR;
            break;
        case InfixOperator::BOOLEAN_AND:
        case InfixOperator::EAGER_AND:
            bin_op = BinOpIR::AND;
            break;
        case InfixOperator::BOOLEAN_EQUAL:
            bin_op = BinOpIR::EQ;
            break;
        case InfixOperator::BOOLEAN_NOT_EQUAL:
            bin_op = BinOpIR::NEQ;
            break;
        case InfixOperator::PLUS:
            bin_op = BinOpIR::ADD;
            break;
        case InfixOperator::MINUS:
            bin_op = BinOpIR::SUB;
            break;
        case InfixOperator::DIVIDE:
            bin_op = BinOpIR::DIV;
            break;
        case InfixOperator::MULTIPLY:
            bin_op = BinOpIR::MUL;
            break;
        case InfixOperator::MODULO:
            bin_op = BinOpIR::MOD;
            break;
        case InfixOperator::LESS_THAN:
            bin_op = BinOpIR::LT;
            break;
        case InfixOperator::GREATER_THAN:
            bin_op = BinOpIR::GT;
            break;
        case InfixOperator::LESS_THAN_EQUAL:
            bin_op = BinOpIR::LEQ;
            break;
        case InfixOperator::GREATER_THAN_EQUAL:
            bin_op = BinOpIR::GEQ;
            break;
    }

    auto expression_ir = std::make_unique<ExpressionIR>(std::in_place_type<BinOpIR>, bin_op, std::move(left), std::move(right));
    return expression_ir;
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(PrefixExpression &expr) {
    assert(expr.expression.get());

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
            // TODO: This can be handled by embedding booleans into the jump code (L16)
            assert(false); // We should never visit this code ideally... (asserting for now)

            // Generate unique labels
            auto true_name = LabelIR::generateName("true");
            auto false_name = LabelIR::generateName("false");

            // Generate unique result temp
            auto result_name = TempIR::generateName();

            // Cond jump
            auto cjump_ir = make_unique<StatementIR>(
                in_place_type<CJumpIR>,
                std::move(expression),
                true_name,
                false_name
            );

            // True label obj
            auto true_label = make_unique<StatementIR>(
                in_place_type<LabelIR>,
                true_name
            );
            // False label obj
            auto false_label = make_unique<StatementIR>(
                in_place_type<LabelIR>,
                false_name
            );

            // Move ZERO on true
            auto true_move = make_unique<StatementIR>(
                in_place_type<MoveIR>,
                std::move(make_unique<ExpressionIR>(in_place_type<TempIR>, result_name)),
                ConstIR::makeZero()
            );
            // Move ONE on false
            auto false_move = make_unique<StatementIR>(
                in_place_type<MoveIR>,
                std::move(make_unique<ExpressionIR>(in_place_type<TempIR>, result_name)),
                ConstIR::makeOne()
            );

            // Create vector for sequence
            std::vector<unique_ptr<StatementIR>> seq_vec;
            seq_vec.push_back(std::move(true_move));    // Move(t, 0)
            seq_vec.push_back(std::move(cjump_ir));     // Cjump(e, true, false)
            seq_vec.push_back(std::move(false_label));  // false:
            seq_vec.push_back(std::move(false_move));   // Move(t, 1)
            seq_vec.push_back(std::move(true_label));   // true:

            // Sequence
            auto seq_ir = make_unique<StatementIR>(
                in_place_type<SeqIR>,
                std::move(seq_vec)
            );

            // Result expression
            auto expr_ir = make_unique<ExpressionIR>(
                in_place_type<TempIR>,
                result_name
            );

            // ESEQ
            auto eseq_ir = make_unique<ExpressionIR>(
                in_place_type<ESeqIR>,
                std::move(seq_ir),
                std::move(expr_ir)
            );

            assert(!true_name.empty());
            assert(!false_name.empty());
            assert(!result_name.empty());

            return eseq_ir;
        }
    }
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(CastExpression &expr) {

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
    assert(expr.class_name.get());

    // Call the appropriate constructor of the class
    THROW_ASTtoIRError("TODO: Deferred to A6 - feature of OOP");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(FieldAccess &expr) {
    assert(expr.expression.get());
    assert(expr.identifier.get());

    THROW_ASTtoIRError("TODO: Deferred to A6 - non-static field");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(MethodInvocation &expr) {
    // assert(expr.parent_expr.get());  // can be null
    assert(expr.method_name.get());

    if ( auto qi = std::get_if<QualifiedIdentifier>(expr.parent_expr.get()) ) {
        // Static methods

        // Args vectors
        vector<unique_ptr<ExpressionIR>> call_args_vec = {};
        for ( auto &arg : expr.arguments ) {
            call_args_vec.push_back(std::move(convert(arg)));
        }

        // Name string
        string name_str;
        for ( auto &identifier : qi->identifiers ) {
            name_str += identifier.name + ".";
        }
        name_str += expr.method_name->name;

        // Name IR
        auto name_ir = make_unique<ExpressionIR>(
            in_place_type<NameIR>,
            name_str
        );

        // Call IR
        auto call_ir = make_unique<ExpressionIR>(
            in_place_type<CallIR>,
            std::move(name_ir),
            std::move(call_args_vec)
        );

        return call_ir;
    } else {
        // Non-static methods
        THROW_ASTtoIRError("TODO: Deferred to A6 - non-static method");
    }
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ArrayAccess &expr) {
    assert(expr.array.get());
    assert(expr.selector.get());

    // Get array in temp
    string array_name = TempIR::generateName();
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
    string selector_name = TempIR::generateName();
    auto get_selector = MoveIR::makeStmt(
        TempIR::makeExpr(selector_name),
        std::move(convert(*expr.selector))
    );

    // Bounds check
    auto inbound_name = LabelIR::generateName("inbounds");
    auto bounds_check = CJumpIR::makeStmt(
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
        std::move(inbound_call)                         // MEM(t_a + 4 + 4*t_i)
    );

    return eseq_ir;
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(QualifiedThis &expr) {
    THROW_ASTtoIRError("QualifiedThis is not supported (I think)");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ArrayCreationExpression &expr) {
    assert(expr.type.get());
    assert(expr.expression.get());

    if ( auto primitive = expr.type->link.getIfIsPrimitive() ) {
        // Primitive type

        // Get inner expression
        auto size_name = TempIR::generateName();
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
        auto error_call = CallIR::makeException();

        // Allocate space
        auto non_negative_label = LabelIR::makeStmt(non_negative_name);
        auto array_name = TempIR::generateName();
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
        auto iterator_name = TempIR::generateName();
        auto start_loop = LabelIR::generateName("start_loop");
        auto exit_loop = LabelIR::generateName("exit_loop");
        auto dummy_name = LabelIR::generateName("dummy");
        vector<unique_ptr<StatementIR>> seq_vec;

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
            // CJump(t_i >= 4 * t_size, exit_loop, start_loop)
            CJumpIR::makeStmt(
                BinOpIR::makeExpr(
                    BinOpIR::GEQ,
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
            // MOVE(MEM(t_a + t_i), 0) 
            MoveIR::makeStmt(
                MemIR::makeExpr(
                    BinOpIR::makeExpr(
                        BinOpIR::ADD,
                        TempIR::makeExpr(array_name),
                        TempIR::makeExpr(iterator_name)
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
    THROW_ASTtoIRError("TODO: Deferred to A6 - qualified identifiers");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(InstanceOfExpression &expr) {
    THROW_ASTtoIRError("TODO: Deferred to A6 - instanceof");
}

std::unique_ptr<ExpressionIR> IRBuilderVisitor::convert(ParenthesizedExpression &expr) {
    assert(expr.expression.get());
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
    assert(stmt.if_clause.get());
    assert(stmt.then_clause.get());

    auto if_true = LabelIR::generateName("if_true");
    auto if_false = LabelIR::generateName("if_false");
   
    vector<unique_ptr<StatementIR>> seq_vec;
    // CJump(expr, true, exit)
    seq_vec.push_back(
        CJumpIR::makeStmt(
            convert(*stmt.if_clause),
            if_true,
            if_false
            #warning False condition does not fall through
        )
    );

    // if_true
    seq_vec.push_back(
        LabelIR::makeStmt(if_true)
    );

    // then_clause
    seq_vec.push_back(
        convert(*stmt.then_clause)
    );

    // if_false
    seq_vec.push_back(
        LabelIR::makeStmt(if_false)
    );

    return SeqIR::makeStmt(std::move(seq_vec));
}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(IfThenElseStatement &stmt) {

}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(WhileStatement &stmt) {

}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ForStatement &stmt) {

}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(Block &stmt) {

}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(EmptyStatement &stmt) {

}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ExpressionStatement &stmt) {

}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(ReturnStatement &stmt) {

}

std::unique_ptr<StatementIR> IRBuilderVisitor::convert(LocalVariableDeclaration &stmt) {

}

/***************************************************************
                            OPERATORS
 ***************************************************************/

void IRBuilderVisitor::operator()(ClassDeclaration &node) {
    // TODO: implement
    
    // CREATE CompUnit
    comp_unit = std::make_unique<CompUnitIR>(node.environment->identifier);
}

void IRBuilderVisitor::operator()(MethodDeclaration &node) {
    // TODO: implement

    // Convert CFG to IR
    if ( node.cfg_start == nullptr ) {
        // CFG does not exist
        // => Method is abstract (ie. no body)
        // => Skip creating IR for method

        assert(node.body.get() == nullptr);
        return;
    }

    // Convert body to a vector of StatementIR's
    std::vector<std::unique_ptr<StatementIR>> statements;
    for ( auto &stmt : node.body->statements ) {
        std::unique_ptr<StatementIR> statementIR = convert(stmt);
        if ( statementIR.get() != nullptr ) {
            statements.emplace_back(std::move(statementIR));
        }
    }

    // CREATE FuncDecl
    // FuncDeclIR func_decl = {
    //     node.environment->identifier,
    //     nullptr,                        // TODO: add body
    //     (int) node.parameters.size()
    // };
}

// Rest of the operators are probably not needed?
