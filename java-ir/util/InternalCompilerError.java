package joosc.util;

/** Exception thrown when the compiler is confused. */
public class InternalCompilerError extends RuntimeException {
    public InternalCompilerError(String msg) {
        super(msg);
    }

    public InternalCompilerError(Throwable cause) {
        this(cause.getMessage(), cause);
    }

    public InternalCompilerError(String msg, Throwable cause) {
        super(msg, cause);
    }
}
