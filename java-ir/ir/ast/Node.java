package joosc.ir.ast;

import joosc.ir.visit.*;

/**
 * A node in an intermediate-representation abstract syntax tree.
 */
public interface Node {
    /**
     * Visit the children of this IR node.
     * @param v the visitor
     * @return the result of visiting children of this node
     */
    Node visitChildren(IRVisitor v);

    InsnMapsBuilder buildInsnMapsEnter(InsnMapsBuilder v);

    Node buildInsnMaps(InsnMapsBuilder v);

    <T> T aggregateChildren(AggregateVisitor<T> v);

    CheckCanonicalIRVisitor checkCanonicalEnter(CheckCanonicalIRVisitor v);

    boolean isCanonical(CheckCanonicalIRVisitor v);

    String label();
}
