package joosc.ir.ast;

import joosc.ir.visit.AggregateVisitor;
import joosc.ir.visit.IRVisitor;

/**
 * An intermediate representation for a move statement
 * MOVE(target, expr)
 */
public class Move extends Stmt {
    private Expr target;
    private Expr src;

    /**
     *
     * @param target the destination of this move
     * @param src the expression whose value is to be moved
     */
    public Move(Expr target, Expr src) {
        this.target = target;
        this.src = src;
    }

    public Move(Expr target, Expr src, boolean trash) {
        this.target = target;
        this.src = src;
    }

    public Expr target() {
        return target;
    }

    public Expr source() {
        return src;
    }

    @Override
    public String label() {
        return "MOVE";
    }

    @Override
    public Node visitChildren(IRVisitor v) {
        Expr target = (Expr) v.visit(this, this.target);
        Expr expr = (Expr) v.visit(this, src);

        if (target != this.target || expr != src)
            return v.nodeFactory().IRMove(target, expr);

        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        T result = v.unit();
        result = v.bind(result, v.visit(target));
        result = v.bind(result, v.visit(src));
        return result;
    }
}
