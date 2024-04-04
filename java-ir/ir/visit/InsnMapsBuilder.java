package joosc.ir.visit;

import joosc.util.InternalCompilerError;
import joosc.ir.ast.Node;

import java.util.HashMap;
import java.util.Map;

public class InsnMapsBuilder extends IRVisitor {
    private Map<String, Integer> nameToIndex;
    private Map<Integer, Node> indexToInsn;

    private int index;

    public InsnMapsBuilder() {
        super(null);
        nameToIndex = new HashMap<>();
        indexToInsn = new HashMap<>();
        index = 0;
    }

    public Map<String, Integer> nameToIndex() {
        return nameToIndex;
    }

    public Map<Integer, Node> indexToInsn() {
        return indexToInsn;
    }

    @Override
    protected IRVisitor enter(Node parent, Node n) {
        InsnMapsBuilder v = n.buildInsnMapsEnter(this);
        return v;
    }

    @Override
    protected Node leave(Node parent, Node n, Node n_, IRVisitor v_) {
        return n_.buildInsnMaps((InsnMapsBuilder) v_);
    }

    public void addInsn(Node n) {
        indexToInsn.put(index, n);
        index++;
    }

    public void addNameToCurrentIndex(String name) {
        if (nameToIndex.containsKey(name))
            throw new InternalCompilerError("Error - encountered "
                    + "duplicate name " + name
                    + " in the IR tree -- go fix the generator.");
        nameToIndex.put(name, index);
    }
}
