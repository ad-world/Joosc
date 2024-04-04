package joosc.ir.ast;

import joosc.ir.visit.AggregateVisitor;
import joosc.ir.visit.CheckCanonicalIRVisitor;
import joosc.ir.visit.IRVisitor;

/**
 * An intermediate representation for an expression evaluated under side effects
 * ESEQ(stmt, expr)
 */
public class ESeq extends Expr_c {
    private Stmt stmt;
    private Expr expr;

    /**
     * @param stmt IR statement to be evaluated for side effects
     * @param expr IR expression to be evaluated after {@code stmt}
     */
    public ESeq(Stmt stmt, Expr expr) {
        this.stmt = stmt;
        this.expr = expr;
    }

    public ESeq(Stmt stmt, Expr expr, boolean replaceParent) {
        this.stmt = stmt;
        this.expr = expr;
    }

    public Stmt stmt() {
        return stmt;
    }

    public Expr expr() {
        return expr;
    }

    @Override
    public String label() {
        return "ESEQ";
    }

    @Override
    public Node visitChildren(IRVisitor v) {
        Stmt stmt = (Stmt) v.visit(this, this.stmt);
        Expr expr = (Expr) v.visit(this, this.expr);

        if (expr != this.expr || stmt != this.stmt)
            return v.nodeFactory().IRESeq(stmt, expr);

        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        T result = v.unit();
        result = v.bind(result, v.visit(stmt));
        result = v.bind(result, v.visit(expr));
        return result;
    }

    @Override
    public boolean isCanonical(CheckCanonicalIRVisitor v) {
        return false;
    }
}
