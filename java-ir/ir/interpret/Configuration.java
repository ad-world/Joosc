package joosc.ir.interpret;

/**
 * Constants for interpreting intermediate code.
 */
public class Configuration {
    /* Some special stack-related names that are used in the IR */
    /** Prefix for argument registers */
    public static final String ABSTRACT_ARG_PREFIX = "_ARG";
    public static final String ABSTRACT_RET = "RET";

    /** Word size; assumes a 32-bit architecture */
    public static final int WORD_SIZE = 4;
}
