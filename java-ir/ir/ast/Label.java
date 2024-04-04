package joosc.ir.ast;

import joosc.ir.visit.InsnMapsBuilder;

/**
 * An intermediate representation for naming a memory address
 */
public class Label extends Stmt {
    private String name;

    /**
     *
     * @param name name of this memory address
     */
    public Label(String name) {
        this.name = name;
    }

    public String name() {
        return name;
    }

    @Override
    public String label() {
        return "LABEL(" + name + ")";
    }

    @Override
    public InsnMapsBuilder buildInsnMapsEnter(InsnMapsBuilder v) {
        v.addNameToCurrentIndex(name);
        return v;
    }
}
