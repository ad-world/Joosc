package joosc.ir.ast;

import joosc.ir.visit.*;

/**
 * A node in an intermediate-representation abstract syntax tree.
 */
public abstract class Node_c implements Node {

    @Override
    public Node visitChildren(IRVisitor v) {
        return this;
    }

    @Override
    public <T> T aggregateChildren(AggregateVisitor<T> v) {
        return v.unit();
    }

    @Override
    public InsnMapsBuilder buildInsnMapsEnter(InsnMapsBuilder v) {
        return v;
    }

    @Override
    public Node buildInsnMaps(InsnMapsBuilder v) {
        v.addInsn(this);
        return this;
    }

    @Override
    public CheckCanonicalIRVisitor checkCanonicalEnter(
            CheckCanonicalIRVisitor v) {
        return v;
    }

    @Override
    public boolean isCanonical(CheckCanonicalIRVisitor v) {
        return true;
    }

    @Override
    public abstract String label();

}
