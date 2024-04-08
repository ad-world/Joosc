#pragma once

#include "IR/ir.h"
#include <vector>
#include <memory>
#include <string>

// Convert IR to Canonical IR form
class IRCanonicalizer {
    // Internally used classes to handle the intermediate result of lowering a statement/expression
    
    struct LoweredStatement {
        std::vector<StatementIR> statements;

        LoweredStatement() = default; 
        LoweredStatement(std::vector<StatementIR> statements);

        template<typename... StatementIRs> LoweredStatement(StatementIRs&&... args) {
            (statements.emplace_back(std::forward<StatementIRs>(args)), ...);
        }
    };

    struct LoweredExpression {
        std::vector<StatementIR> statements;
        std::unique_ptr<ExpressionIR> expression;

        LoweredExpression() = default;
        LoweredExpression(std::vector<StatementIR> statements, std::unique_ptr<ExpressionIR> expression);
        LoweredExpression(std::vector<StatementIR> statements, ExpressionIR expression);

        template<typename... StatementIRs> LoweredExpression(ExpressionIR expression, StatementIRs&&... args) {
            this->expression = std::make_unique<ExpressionIR>(std::move(expression));
            (statements.emplace_back(std::forward<StatementIRs>(args)), ...);
        }
    };

    static inline std::string ARG_TEMPORARY_PREFIX = "CANON_ARG";

  public:
    void operator()(IR&);
    void convert(IR&);

    // Return lowered version of the expression
    LoweredExpression operator()(ExpressionIR&);
    LoweredExpression convert(ExpressionIR&);

    // Return lowered version of the statement
    LoweredStatement operator()(StatementIR&);
    LoweredStatement convert(StatementIR&);
};
