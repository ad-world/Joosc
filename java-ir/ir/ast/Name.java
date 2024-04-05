package joosc.ir.ast;

/**
 * An intermediate representation for named memory address
 * NAME(n)
 */
public class Name extends Expr_c {
    private String name;

    /**
     *
     * @param name name of this memory address
     */
    public Name(String name) {
        this.name = name;
    }

    public String name() {
        return name;
    }

    @Override
    public String label() {
        return "NAME(" + name + ")";
    }
}
