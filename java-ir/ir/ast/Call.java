package joosc.ir.ast;

import joosc.ir.visit.AggregateVisitor;
import joosc.ir.visit.CheckCanonicalIRVisitor;
import joosc.ir.visit.IRVisitor;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * An intermediate representation for a function call
 * CALL(e_target, e_1, ..., e_n)
 */
public class Call extends Expr_c {
    protected Expr target;
    protected List<Expr> args;

    /**
     *
     * @param target address of the code for this function call
     * @param args arguments of this function call
     */
    public Call(Expr target, Expr... args) {
        this(target, Arrays.asList(args));
    }

    /**
     *
     * @param target address of the code for this function call
     * @param numRets number of return values for this function call
     * @param args arguments of this function call
     */
    public Call(Expr target, List<Expr> args) {
        this.target = target;
        this.args = args;
    }

    public Expr target() {
        return target;
    }

    public List<Expr> args() {
        return args;
    }

    public int getNumArgs() {
        return args.size();
    }

    @Override
    public String label() {
        return "CALL";
    }

    @Override
    public Node visitChildren(IRVisitor v) {
        boolean modified = false;

        Expr target = (Expr) v.visit(this, this.target);
        if (target != this.target) modified = true;

        List<Expr> results = new ArrayList<>(args.size());
        for (Expr arg : args) {
            Expr newExpr = (Expr) v.visit(this, arg);
            if (newExpr != arg) modified = true;
            results.add(newExpr);
        }

        if (modified) return v.nodeFactory().IRCall(target, results);

        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        T result = v.unit();
        result = v.bind(result, v.visit(target));
        for (Expr arg : args)
            result = v.bind(result, v.visit(arg));
        return result;
    }

    @Override
    public boolean isCanonical(CheckCanonicalIRVisitor v) {
        return true;
        // return !v.inExpr();
    }
}
