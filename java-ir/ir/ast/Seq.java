package joosc.ir.ast;

import joosc.ir.visit.AggregateVisitor;
import joosc.ir.visit.CheckCanonicalIRVisitor;
import joosc.ir.visit.IRVisitor;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

/**
 * An intermediate representation for a sequence of statements
 * SEQ(s1,...,sn)
 */
public class Seq extends Stmt {
    private List<Stmt> stmts;
    private boolean replaceParent;

    /**
     * @param stmts the statements
     */
    public Seq(Stmt... stmts) {
        this(Arrays.asList(stmts));
    }

    private <T> List<T> filterNulls(List<T> list) {
        return list
                .stream()
                .filter(Objects::nonNull)
                .collect(Collectors.toList());
    }
    /**
     * Create a SEQ from a list of statements.
     * The list should not be modified subsequently.
     * @param stmts the sequence of statements
     */
    public Seq(List<Stmt> stmts) {
        // filter out nulls
        this.stmts = filterNulls(stmts);
        this.replaceParent = false;
    }

    public Seq(List<Stmt> stmts, boolean replaceParent) {
        this.stmts = filterNulls(stmts);
        this.replaceParent = replaceParent;
    }

    public List<Stmt> stmts() {
        return stmts;
    }

    @Override
    public String label() {
        return "SEQ";
    }

    @Override
    public Node visitChildren(IRVisitor v) {
        boolean modified = false;

        List<Stmt> results = new ArrayList<>(stmts.size());
        for (Stmt stmt : stmts) {
            Stmt newStmt = (Stmt) v.visit(this, stmt);
            if (newStmt != stmt) modified = true;
            results.add(newStmt);
        }

        if (modified) return v.nodeFactory().IRSeq(results);

        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        T result = v.unit();
        for (Stmt stmt : stmts)
            result = v.bind(result, v.visit(stmt));
        return result;
    }

    @Override
    public CheckCanonicalIRVisitor checkCanonicalEnter(
            CheckCanonicalIRVisitor v) {
        return v.enterSeq();
    }

    @Override
    public boolean isCanonical(CheckCanonicalIRVisitor v) {
        return !v.inSeq();
    }
}
