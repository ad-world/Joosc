package joosc.ir.ast;

import java.util.List;
import java.util.Map;

public interface NodeFactory {

    BinOp IRBinOp(BinOp.OpType type, Expr left, Expr right);

    /**
     *
     * @param target address of the code for this function call
     * @param args arguments of this function call
     */
    Call IRCall(Expr target, Expr... args);

    /**
     *
     * @param target address of the code for this function call
     * @param args arguments of this function call
     */
    Call IRCall(Expr target, List<Expr> args);

    /**
     * Construct a CJUMP instruction with fall-through on false.
     * @param expr the condition for the jump
     * @param trueLabel the destination of the jump if {@code expr} evaluates
     *          to true
     */
    CJump IRCJump(Expr expr, String trueLabel);

    /**
     *
     * @param expr the condition for the jump
     * @param trueLabel the destination of the jump if {@code expr} evaluates
     *          to true
     * @param falseLabel the destination of the jump if {@code expr} evaluates
     *          to false
     */
    CJump IRCJump(Expr expr, String trueLabel, String falseLabel);

    CompUnit IRCompUnit(String name);

    CompUnit IRCompUnit(String name, Map<String, FuncDecl> functions);

    /**
     *
     * @param value value of this constant
     */
    Const IRConst(int value);

    /**
     *
     * @param stmt IR statement to be evaluated for side effects
     * @param expr IR expression to be evaluated after {@code stmt}
     */
    ESeq IRESeq(Stmt stmt, Expr expr);

    /**
     *
     * @param expr the expression to be evaluated and result discarded
     */
    Exp IRExp(Expr expr);

    FuncDecl IRFuncDecl(String name, int numParams, Stmt stmt);

    /**
     *
     * @param expr the destination of the jump
     */
    Jump IRJump(Expr expr);

    /**
     *
     * @param name name of this memory address
     */
    Label IRLabel(String name);

    /**
     *
     * @param expr the address of this memory location
     */
    Mem IRMem(Expr expr);

    /**
     *
     * @param target the destination of this move
     * @param expr the expression whose value is to be moved
     */
    Move IRMove(Expr target, Expr expr);

    /**
     *
     * @param name name of this memory address
     */
    Name IRName(String name);

    /**
     * @param ret value to return
     */
    Return IRReturn(Expr ret);

    /**
     * @param stmts the statements
     */
    Seq IRSeq(Stmt... stmts);

    /**
     * Create a SEQ from a list of statements.
     * The list should not be modified subsequently.
     * @param stmts the sequence of statements
     */
    Seq IRSeq(List<Stmt> stmts);

    /**
     *
     * @param name name of this temporary register
     */
    Temp IRTemp(String name);
}
