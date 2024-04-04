package joosc.ir.ast;

import joosc.ir.visit.AggregateVisitor;
import joosc.ir.visit.IRVisitor;
import joosc.ir.visit.InsnMapsBuilder;

/** An IR function declaration */
public class FuncDecl extends Node_c {
    private String name;
    private Stmt body;
    private int numParams;

    public FuncDecl(String name, int numParams, Stmt body) {
        this.name = name;
        this.body = body;
        this.numParams = numParams;
    }

    public String name() {
        return name;
    }

    public Stmt body() {
        return body;
    }

    public int getNumParams() {
        return numParams;
    }

    @Override
    public String label() {
        return "FUNC " + name;
    }

    @Override
    public Node visitChildren(IRVisitor v) {
        Stmt stmt = (Stmt) v.visit(this, body);

        if (stmt != body) return v.nodeFactory().IRFuncDecl(
                name, numParams, stmt
        );

        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        T result = v.unit();
        result = v.bind(result, v.visit(body));
        return result;
    }

    @Override
    public InsnMapsBuilder buildInsnMapsEnter(InsnMapsBuilder v) {
        v.addNameToCurrentIndex(name);
        v.addInsn(this);
        return v;
    }

    @Override
    public Node buildInsnMaps(InsnMapsBuilder v) {
        return this;
    }
}
