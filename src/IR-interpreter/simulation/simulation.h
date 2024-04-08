#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include "IR/ir.h"
#include <vector>
#include <stack>
#include "IR-interpreter/expr-stack/expr-stack.h"
#include "IR-interpreter/util/errors.h"
#include "IR-interpreter/maps-builder/maps-builder.h"

extern int WORD_SIZE;


/**
 * A simple IR interpreter
 */
class Simulator {
    /** compilation unit to be interpreted */
    CompUnitIR *compUnit;    

    /** map from labeled name to address */
    std::unordered_map<std::string, int> nameToIndex;

    /** heap */
    std::vector<int> memory;
    
    /** heap size maximum **/
    int heapSizeMax;

    ExprStack exprStack;
    std::unordered_set<std::string> libraryFunctions;

    /**
     *
     * @param name name of the label
     * @return the IR node at the named label
     */
    int findLabel(std::string label);

    /**
     * Get the IR node at the given address
     * @param ptr the I
     * @return the IR node at the given address
     */
    IR* getIRFromPtr(IR_PTR ptr);

    /**
     * Holds the instruction pointer and temporary registers
     * within an execution frame.
     */
    class ExecutionFrame {
        /** parent object */
        Simulator& parent;
        /** local registers (register name -> value) */
        std::unordered_map<std::string, int> regs;
    protected:  
        /** instruction pointer */
        int ip;

    public:
        /** return value from this frame */
        int ret;
        ExecutionFrame(int ip, Simulator& parent);

         /**
         * Fetch the value at the given register
         * @param tempName name of the register
         * @return the value at the given register
         */
        int get(std::string tempName);


        /**
         * Store a value into the given register
         * @param tempName name of the register
         * @param value value to be stored
         */
        void put(std::string tempName, int value);

        /**
         * Advance the instruction pointer. Since we're dealing with a tree,
         * this is postorder traversal, one step at a time, modulo jumps.
         */
        bool advance();

        /**
         * Set the instruction pointer to the given value
         * @param ip the new instruction pointer
        */
        void setIP(int ip);

        /**
         * Get the Current Node object, based on the current ip
         * @return IR_PTR the pointer of the IR node at the current ip
         */
        IR_PTR getCurrentNode();    


    };
protected:
    /** map from address to instruction */
    std::unordered_map<int, IR_PTR> indexToNode;
    int debugLevel = 0;

    int getMemoryIndex(int addr);

    /**
     * Simulate a library function call, returning the list of returned values
     * @param name name of the function call
     * @param args arguments to the function call, which may include
     *          the pointer to the location of multiple results
     */
    int libraryCall(std::string name, std::vector<int> args);
    void leave(ExecutionFrame *frame);

public:
    static const int DEFAULT_HEAP_SIZE = 2048;

    /**
     * Construct an IR interpreter
     * @param compUnit the compilation unit to be interpreted
     * @param heapSize the heap size
     */
    Simulator(IR *compUnit, int heapSizeMax = DEFAULT_HEAP_SIZE);

     /**
     * Construct an IR interpreter with a default heap size
     * @param compUnit the compilation unit to be interpreted
     */
    // Simulator(Simulator& other); not needed for now?

    /**
     * Allocate a specified amount of bytes on the heap
     * @param size the number of bytes to be allocated
     * @return the starting address of the allocated region on the heap
     */
    int malloc(int size);

      /**
     * Allocate a specified amount of bytes on the heap and initialize them to 0.
     * @param size the number of bytes to be allocated
     * @return the starting address of the allocated region on the heap
     */
    int calloc(int size);

     /**
     * Read a value at the specified location on the heap
     * @param addr the address to be read
     * @return the value at {@code addr}
     */
    int read(int addr);

    /**
     * Write a value at the specified location on the heap
     * @param addr the address to be written
     * @param value the value to be written
     */
    void store(int addr, int value);

     /**
     * Simulate a function call
     * All arguments to the function call are passed via registers with prefix
     * {@link Configuration#ABSTRACT_ARG_PREFIX} and indices starting from 0.
     * @param name name of the function call
     * @param args arguments to the function call
     * @return the value that would be in register
     *          {@link Configuration#ABSTRACT_RET}
     */
    int call(std::string name, std::vector<int> args);

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
    int call(ExecutionFrame& parent, std::string name, std::vector<int> args);

    /**
     * Set the Debug Level object
     * 
     * @param debugLevel 
     */
    void setDebugLevel(int debugLevel);

    /**
     * Return a string representation of the current node type
     * 
     * @param node 
     * @return std::string a string representation of the current node type 
     */
    std::string getNodeType(IR_PTR node);
};