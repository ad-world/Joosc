#include "IR/ir.h"
#include <string>

class BinOpIR {
    enum OpType {
        ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT,
        EQ, NEQ, LT, GT, LEQ, GEQ
    };

    OpType op;
    ExpressionIR *left;
    ExpressionIR *right;
public:
    BinOpIR(OpType op, ExpressionIR *left, ExpressionIR *right) : op(op), left(left), right(right) {}
    OpType opType() { return op; }
    ExpressionIR *left() { return left; }
    ExpressionIR *right() { return right; }
    
    bool isConstant() {
        // TODO: implement helpers to determine constant of IRExpression variatn, return left->isConstant() && right->isConstant();
        return false;
    }

};