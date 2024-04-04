package joosc.ir.ast;

import joosc.ir.visit.AggregateVisitor;
import joosc.ir.visit.IRVisitor;

/**
 * An intermediate representation for a transfer of control
 */
public class Jump extends Stmt {
    private Expr target;

    /**
     *
     * @param expr the destination of the jump
     */
    public Jump(Expr expr) {
        target = expr;
    }

    public Expr target() {
        return target;
    }

    @Override
    public String label() {
        return "JUMP";
    }

    @Override
    public Node visitChildren(IRVisitor v) {
        Expr expr = (Expr) v.visit(this, target);

        if (expr != target) return v.nodeFactory().IRJump(expr);

        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        T result = v.unit();
        result = v.bind(result, v.visit(target));
        return result;
    }
}
