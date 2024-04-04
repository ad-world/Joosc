package joosc.ir.ast;

import joosc.ir.visit.AggregateVisitor;
import joosc.ir.visit.IRVisitor;

import java.util.LinkedHashMap;
import java.util.Map;

/**
 * An intermediate representation for a compilation unit
 */
public class CompUnit extends Node_c {
    private String name;
    private Map<String, FuncDecl> functions;

    public CompUnit(String name) {
        this.name = name;
        functions = new LinkedHashMap<>();
    }

    public CompUnit(String name, Map<String, FuncDecl> functions) {
        this.name = name;
        this.functions = functions;
    }

    public void appendFunc(FuncDecl func) {
        functions.put(func.name(), func);
    }

    public String name() {
        return name;
    }

    public Map<String, FuncDecl> functions() {
        return functions;
    }

    public FuncDecl getFunction(String name) {
        return functions.get(name);
    }

    @Override
    public String label() {
        return "COMPUNIT";
    }

    @Override
    public Node visitChildren(IRVisitor v) {
        boolean modified = false;

        Map<String, FuncDecl> results = new LinkedHashMap<>();
        for (FuncDecl func : functions.values()) {
            FuncDecl newFunc = (FuncDecl) v.visit(this, func);
            if (newFunc != func) modified = true;
            results.put(newFunc.name(), newFunc);
        }

        if (modified) return v.nodeFactory().IRCompUnit(name, results);

        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        T result = v.unit();
        for (FuncDecl func : functions.values())
            result = v.bind(result, v.visit(func));
        return result;
    }
}
