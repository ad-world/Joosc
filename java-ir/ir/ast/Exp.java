package joosc.ir.ast;

import joosc.ir.visit.AggregateVisitor;
import joosc.ir.visit.CheckCanonicalIRVisitor;
import joosc.ir.visit.IRVisitor;

/**
 * An intermediate representation for evaluating an expression for side effects,
 * discarding the result
 * EXP(e)
 */
public class Exp extends Stmt {
    private Expr expr;

    /**
     *
     * @param expr the expression to be evaluated and result discarded
     */
    public Exp(Expr expr) {
        this.expr = expr;
    }

    public Expr expr() {
        return expr;
    }

    @Override
    public String label() {
        return "EXP";
    }

    @Override
    public Node visitChildren(IRVisitor v) {
        Expr expr = (Expr) v.visit(this, this.expr);

        if (expr != this.expr) return v.nodeFactory().IRExp(expr);

        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        T result = v.unit();
        result = v.bind(result, v.visit(expr));
        return result;
    }

    @Override
    public CheckCanonicalIRVisitor checkCanonicalEnter(
            CheckCanonicalIRVisitor v) {
        return v.enterExp();
    }
}
