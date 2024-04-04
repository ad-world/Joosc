package joosc.ir.ast;

/**
 * An intermediate representation for a 64-bit integer constant.
 * CONST(n)
 */
public class Const extends Expr_c {
    private int value;

    /**
     *
     * @param value value of this constant
     */
    public Const(int value) {
        this.value = value;
    }

    public int value() {
        return value;
    }

    @Override
    public String label() {
        return "CONST(" + value + ")";
    }

    @Override
    public boolean isConstant() {
        return true;
    }

    @Override
    public int constant() {
        return value;
    }
}
