package joosc.ir.interpret;

import joosc.util.InternalCompilerError;
import joosc.ir.ast.*;
import joosc.ir.visit.InsnMapsBuilder;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.*;

/**
 * A simple IR interpreter
 */
public class Simulator {
    /** compilation unit to be interpreted */
    private CompUnit compUnit;

    /** map from address to instruction */
    protected Map<Integer, Node> indexToInsn;

    /** map from labeled name to address */
    private Map<String, Integer> nameToIndex;

    /** a random number generator for initializing garbage */
    protected Random r;

    /** heap */
    private ArrayList<Integer> mem;

    /** heap size maximum **/
    private int heapSizeMax;

    private ExprStack exprStack;
    private BufferedReader inReader;

    private Set<String> libraryFunctions;

    protected static int debugLevel = 0;

    public static final int DEFAULT_HEAP_SIZE = 2048;

    /**
     * Construct an IR interpreter with a default heap size
     * @param compUnit the compilation unit to be interpreted
     */
    public Simulator(CompUnit compUnit) {
        this(compUnit, DEFAULT_HEAP_SIZE);
    }

    /**
     * Construct an IR interpreter
     * @param compUnit the compilation unit to be interpreted
     * @param heapSize the heap size
     */
    public Simulator(CompUnit compUnit, int heapSize) {
        this.compUnit = compUnit;
        this.heapSizeMax = heapSize;

        r = new Random();

        mem = new ArrayList<>();

        exprStack = new ExprStack();
        inReader = new BufferedReader(new InputStreamReader(System.in));

        libraryFunctions = new LinkedHashSet<>();
        libraryFunctions.add("__malloc");
        libraryFunctions.add("__debexit");
        libraryFunctions.add("__exception");
        libraryFunctions.add("NATIVEjava.io.OutputStream.nativeWrite");

        InsnMapsBuilder imb = new InsnMapsBuilder();
        compUnit = (CompUnit) imb.visit(compUnit);
        indexToInsn = imb.indexToInsn();
        nameToIndex = imb.nameToIndex();
    }

    /**
     * Allocate a specified amount of bytes on the heap
     * @param size the number of bytes to be allocated
     * @return the starting address of the allocated region on the heap
     */
    public int malloc(int size) {
        if (size < 0) throw new Trap("Invalid size");
        if (size % Configuration.WORD_SIZE != 0)
            throw new Trap("Can only allocate in chunks of "
                    + Configuration.WORD_SIZE + " bytes!");

        int retval = mem.size();
        if (retval + size > heapSizeMax) throw new Trap("Out of heap!");
        for (int i = 0; i < size; i++) {
            mem.add(r.nextInt());
        }
        return retval;
    }

    /**
     * Allocate a specified amount of bytes on the heap and initialize them to 0.
     * @param size the number of bytes to be allocated
     * @return the starting address of the allocated region on the heap
     */
    public int calloc(int size) {
        int retval = malloc(size);
        for (int i = (int) retval; i < retval + size; i++) {
            mem.set(i, 0);
        }
        return retval;
    }

    /**
     * Read a value at the specified location on the heap
     * @param addr the address to be read
     * @return the value at {@code addr}
     */
    public int read(int addr) {
        int i = getMemoryIndex(addr);
        if (i >= mem.size())
            throw new Trap("Attempting to read past end of heap");
        return mem.get(i);
    }

    /**
     * Write a value at the specified location on the heap
     * @param addr the address to be written
     * @param value the value to be written
     */
    public void store(int addr, int value) {
        int i = getMemoryIndex(addr);
        if (i >= mem.size())
            throw new Trap("Attempting to store past end of heap");
        mem.set(i, value);
    }

    protected int getMemoryIndex(int addr) {
        if (addr % Configuration.WORD_SIZE != 0)
            throw new Trap("Unaligned memory access: " + addr + " (word size="
                    + Configuration.WORD_SIZE + ")");
        return addr / Configuration.WORD_SIZE;
    }

    /**
     * Simulate a function call
     * All arguments to the function call are passed via registers with prefix
     * {@link Configuration#ABSTRACT_ARG_PREFIX} and indices starting from 0.
     * @param name name of the function call
     * @param args arguments to the function call
     * @return the value that would be in register
     *          {@link Configuration#ABSTRACT_RET}
     */
    public int call(String name, int... args) {
        return call(new ExecutionFrame(-1), name, args);
    }

    /**
     * Simulate a function call.
     * All arguments to the function call are passed via registers with prefix
     * {@link Configuration#ABSTRACT_ARG_PREFIX} and indices starting from 0.
     * The function call should return the result via register
     * {@link Configuration#ABSTRACT_RET}.
     * @param parent parent call frame to write _RET values to
     * @param name name of the function call
     * @param args arguments to the function call
     * @return the value of register
     *          {@link Configuration#ABSTRACT_RET}
     */
    public int call(ExecutionFrame parent, String name, int... args) {
        final int ret;
        // Catch standard library calls.
        if (libraryFunctions.contains(name)) {
            ret = libraryCall(name, args);
        } else {
            FuncDecl fDecl = compUnit.getFunction(name);
            if (fDecl == null)
                throw new InternalCompilerError("Tried to call an unknown function: '"
                        + name + "'");

            // Create a new stack frame.
            int ip = findLabel(name);
            ExecutionFrame frame = new ExecutionFrame(ip);

            // Pass the remaining arguments into registers.
            for (int i = 0; i < args.length; ++i)
                frame.put(Configuration.ABSTRACT_ARG_PREFIX + i, args[i]);

            // Simulate!
            while (frame.advance()) ;

            ret = frame.ret;
        }

        parent.put(Configuration.ABSTRACT_RET, ret);
        return ret;
    }

    /**
     * Simulate a library function call, returning the list of returned values
     * @param name name of the function call
     * @param args arguments to the function call, which may include
     *          the pointer to the location of multiple results
     */
    protected int libraryCall(String name, int[] args) {
        final int ws = Configuration.WORD_SIZE;
        final int ret;
        switch (name) {
            case "NATIVEjava.io.OutputStream.nativeWrite": {
                System.out.print((char) (byte) args[0]);
                ret = 0;
                break;
            }
            case "__malloc": {
                ret = malloc(args[0]);
                break;
            }
            case "__debexit": {
                System.exit(args[0]);
                ret = args[0];
                break;
            }
            case "__exception": {
                System.exit(13);
                ret = 13;
                break;
            }
            default:
                throw new InternalCompilerError("Unsupported library function: "
                        + name);
        }
        return ret;
    }

    protected void leave(ExecutionFrame frame) {
        Node insn = frame.getCurrentInsn();
        if (insn instanceof Const)
            exprStack.pushValue(((Const) insn).value());
        else if (insn instanceof Temp) {
            String tempName = ((Temp) insn).name();
            exprStack.pushTemp(frame.get(tempName), tempName);
        }
        else if (insn instanceof BinOp) {
            int r = exprStack.popValue();
            int l = exprStack.popValue();
            int result;
            switch (((BinOp) insn).opType()) {
            case ADD:
                result = l + r;
                break;
            case SUB:
                result = l - r;
                break;
            case MUL:
                result = l * r;
                break;
            case DIV:
                if (r == 0) throw new Trap("Division by zero!");
                result = l / r;
                break;
            case MOD:
                if (r == 0) throw new Trap("Division by zero!");
                result = l % r;
                break;
            case AND:
                result = l & r;
                break;
            case OR:
                result = l | r;
                break;
            case XOR:
                result = l ^ r;
                break;
            case LSHIFT:
                result = l << r;
                break;
            case RSHIFT:
                result = l >>> r;
                break;
            case ARSHIFT:
                result = l >> r;
                break;
            case EQ:
                result = l == r ? 1 : 0;
                break;
            case NEQ:
                result = l != r ? 1 : 0;
                break;
            case LT:
                result = l < r ? 1 : 0;
                break;
            case GT:
                result = l > r ? 1 : 0;
                break;
            case LEQ:
                result = l <= r ? 1 : 0;
                break;
            case GEQ:
                result = l >= r ? 1 : 0;
                break;
            default:
                throw new InternalCompilerError("Invalid binary operation");
            }
            exprStack.pushValue(result);
        }
        else if (insn instanceof Mem) {
            int addr = exprStack.popValue();
            exprStack.pushAddr(read(addr), addr);
        }
        else if (insn instanceof Call) {
            int argsCount = ((Call) insn).args().size();
            int args[] = new int[argsCount];
            for (int i = argsCount - 1; i >= 0; --i)
                args[i] = exprStack.popValue();
            StackItem target = exprStack.pop();
            String targetName;
            if (target.type == StackItem.Kind.NAME)
                targetName = target.name;
            else if (indexToInsn.containsKey(target.value)) {
                Node node = indexToInsn.get(target.value);
                if (node instanceof FuncDecl)
                    targetName = ((FuncDecl) node).name();
                else throw new InternalCompilerError("Call to a non-function instruction!");
            }
            else throw new InternalCompilerError("Invalid function call '"
                    + insn + "' (target '" + target.value + "' is unknown)!");

            int retVal = call(frame, targetName, args);
            exprStack.pushValue(retVal);
        }
        else if (insn instanceof Name) {
            String name = ((Name) insn).name();
            exprStack.pushName(libraryFunctions.contains(name)
                    ? -1 : findLabel(name), name);
        }
        else if (insn instanceof Move) {
            int r = exprStack.popValue();
            StackItem stackItem = exprStack.pop();
            switch (stackItem.type) {
            case MEM:
                if (debugLevel > 0)
                    System.out.println("mem[" + stackItem.addr + "]=" + r);
                store(stackItem.addr, r);
                break;
            case TEMP:
                if (debugLevel > 0)
                    System.out.println("temp[" + stackItem.temp + "]=" + r);
                frame.put(stackItem.temp, r);
                break;
            default:
                throw new InternalCompilerError("Invalid MOVE!");
            }
        }
        else if (insn instanceof Exp) {
            // Discard result.
            exprStack.pop();
        }
        else if (insn instanceof Jump)
            frame.setIP(exprStack.popValue());
        else if (insn instanceof CJump) {
            CJump irCJump = (CJump) insn;
            int top = exprStack.popValue();
            String label;
            if (top == 0)
                label = irCJump.falseLabel();
            else if (top == 1)
                label = irCJump.trueLabel();
            else throw new InternalCompilerError("Invalid value in CJUMP: expected 0/1, got "
                    + top);
            if (label != null) frame.setIP(findLabel(label));
        }
        else if (insn instanceof Return) {
            frame.ret = exprStack.popValue();
            frame.setIP(-1);
        }
    }

    /**
     *
     * @param name name of the label
     * @return the IR node at the named label
     */
    private int findLabel(String name) {
        if (!nameToIndex.containsKey(name))
            throw new Trap("Could not find label '" + name + "'!");
        return nameToIndex.get(name);
    }

    /**
     * Holds the instruction pointer and temporary registers
     * within an execution frame.
     */
    private class ExecutionFrame {
        /** instruction pointer */
        protected int ip;

        /** return value from this frame */
        protected int ret;

        /** local registers (register name -> value) */
        private Map<String, Integer> regs;

        public ExecutionFrame(int ip) {
            this.ip = ip;
            regs = new HashMap<>();
            ret = r.nextInt();
        }

        /**
         * Fetch the value at the given register
         * @param tempName name of the register
         * @return the value at the given register
         */
        public int get(String tempName) {
            if (!regs.containsKey(tempName)) {
                /* Referencing a temp before having written to it - initialize
                   with garbage */
                put(tempName, r.nextInt());
            }
            return regs.get(tempName);
        }

        /**
         * Store a value into the given register
         * @param tempName name of the register
         * @param value value to be stored
         */
        public void put(String tempName, int value) {
            regs.put(tempName, value);
        }

        /**
         * Advance the instruction pointer. Since we're dealing with a tree,
         * this is postorder traversal, one step at a time, modulo jumps.
         */
        public boolean advance() {
            // Time out if necessary.
            if (Thread.currentThread().isInterrupted()) return false;

            if (debugLevel > 1)
                System.out.println("Evaluating " + getCurrentInsn().label());
            int backupIP = ip;
            leave(this);

            if (ip == -1) return false; /* RETURN */

            if (ip != backupIP) /* A jump was performed */
                return true;

            ip++;
            return true;
        }

        public void setIP(int ip) {
            this.ip = ip;
            if (debugLevel > 1) {
                if (ip == -1)
                    System.out.println("Returning");
                else System.out.println("Jumping to "
                        + getCurrentInsn().label());
            }
        }

        public Node getCurrentInsn() {
            Node insn = indexToInsn.get(ip);
            if (insn == null)
                throw new Trap("No next instruction.  Forgot RETURN?");
            return insn;
        }
    }

    /**
     * While traversing the IR tree, we require a stack in order to hold
     * a number of single-word values (e.g., to evaluate binary expressions).
     * This also keeps track of whether a value was created by a TEMP
     * or MEM, or NAME reference, which is useful when executing moves.
     */
    private static class ExprStack {

        private Stack<StackItem> stack;

        public ExprStack() {
            stack = new Stack<>();
        }

        public int popValue() {
            int value = stack.pop().value;
            if (debugLevel > 1) System.out.println("Popping value " + value);
            return value;
        }

        public StackItem pop() {
            return stack.pop();
        }

        public void pushAddr(int value, int addr) {
            if (debugLevel > 1)
                System.out.println("Pushing MEM " + value + " (" + addr + ")");
            stack.push(new StackItem(value, addr));
        }

        public void pushTemp(int value, String temp) {
            if (debugLevel > 1)
                System.out.println("Pushing TEMP " + value + " (" + temp + ")");
            stack.push(new StackItem(StackItem.Kind.TEMP, value, temp));
        }

        public void pushName(int value, String name) {
            if (debugLevel > 1)
                System.out.println("Pushing NAME " + value + " (" + name + ")");
            stack.push(new StackItem(StackItem.Kind.NAME, value, name));
        }

        public void pushValue(int value) {
            if (debugLevel > 1) System.out.println("Pushing value " + value);
            stack.push(new StackItem(value));
        }
    }

    public static class StackItem {
        public enum Kind {
            COMPUTED, MEM, TEMP, NAME
        }

        public Kind type;
        public int value;
        public int addr;
        public String temp;
        public String name;

        public StackItem(int value) {
            type = Kind.COMPUTED;
            this.value = value;
        }

        public StackItem(int value, int addr) {
            type = Kind.MEM;
            this.value = value;
            this.addr = addr;
        }

        public StackItem(Kind type, int value, String string) {
            this.type = type;
            this.value = value;
            if (type == Kind.TEMP)
                temp = string;
            else if (type == Kind.NAME)
                name = string;
            else
                throw new InternalCompilerError("Unknown StackItem Kind");
        }
    }

    public static class Trap extends RuntimeException {
        public Trap(String message) {
            super(message);
        }
    }

    public static class OutOfBoundTrap extends Trap {
        public OutOfBoundTrap(String message) {
            super(message);
        }
    }
}
