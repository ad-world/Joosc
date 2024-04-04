package joosc.ir.visit;

import joosc.util.Copy;
import joosc.util.InternalCompilerError;
import joosc.ir.ast.Node;
import joosc.ir.ast.NodeFactory;

public abstract class IRVisitor implements Copy<IRVisitor> {

    protected NodeFactory inf;

    public IRVisitor(NodeFactory inf) {
        this.inf = inf;
    }

    public NodeFactory nodeFactory() {
        return inf;
    }

    /**
     * Recursively traverse the IR subtree rooted at {@code n}
     */
    public Node visit(Node parent, Node n) {
        if (n == null) return null;

        /* Allow the visitor implementation to hijack traversal of n */
        Node overrideValue = override(parent, n);
        if (overrideValue != null) return overrideValue;

        IRVisitor v_ = enter(parent, n);
        if (v_ == null)
            throw new InternalCompilerError("IRVisitor.enter() returned null!");

        Node n_ = n.visitChildren(v_);
        if (n_ == null)
            throw new InternalCompilerError("IRVisitor.visitChildren() returned null!");

        n_ = leave(parent, n, n_, v_);
        if (n_ == null)
            throw new InternalCompilerError("IRVisitor.leave() returned null!");

        return n_;
    }

    /**
     * Recursively traverse the IR subtree rooted at {@code n}
     */
    public Node visit(Node node) {
        return visit(null, node);
    }

    /**
     * Allows to hijack the traversal of a subtree. This function is called by
     * {@link #visit(Node, Node)} upon entering node {@code n}.
     * If a non-null node {@code n0} is returned, the traversal is stopped
     * and the resulting AST has {@code n0} in place of {@code n}.
     *
     * By default, overriding is inactive.
     */
    protected Node override(Node parent, Node n) {
        return null;
    }

    /**
     * Called upon entering {@code n} during the AST traversal. This allows
     * to perform certain actions, including returning a new Node visitor to be
     * used in the subtree.
     */
    protected IRVisitor enter(Node parent, Node n) {
        return this;
    }

    /**
     * Called after finishing traversal of the subtree rooted at {@code n}.
     * When {@link #enter(Node, Node)} creates a new visitor to be used on
     * the subtree, the old visitor still receives the call to {@code leave()}
     * -- that is, {@code leave()} always executed the same number of times
     * as {@link #enter(Node, Node)}.
     * This node provides the final opportunity of placing an updated node
     * in the output AST.
     *
     * @param parent
     *            The parent AST node of {@code n} or {@code null}
     *            when it is the root.
     * @param n
     *            The original node in the input AST
     * @param n_
     *            The node returned by {@link Node#visitChildren(IRVisitor)}
     * @param v_
     *            The new node visitor created by
     *            {@link #enter(Node, Node)}, or {@code this}.
     */

    protected Node leave(Node parent, Node n, Node n_, IRVisitor v_) {
        return n_;
    }

    /**
     * Return a clone of this visitor if the given visitor is this visitor,
     * or the given visitor otherwise.
     * @param v the visitor
     * @return a clone of v if v == this, or v otherwise
     */
    protected <V extends IRVisitor> V copyIfNeeded(V v) {
        if (v == this) return Copy.Util.copy(v);
        return v;
    }

    @Override
    public IRVisitor copy() {
        try {
            return (IRVisitor) super.clone();
        }
        catch (CloneNotSupportedException e) {
            throw new InternalCompilerError("Java clone() weirdness.");
        }
    }
}
