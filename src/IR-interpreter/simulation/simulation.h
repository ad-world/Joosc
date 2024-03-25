#include <unordered_map>
#include <unordered_set>
#include <string>
#include "IR/ir.h"
#include <vector>
#include <stack>
#include "IR-interpreter/expr-stack/expr-stack.h"
#include "IR-interpreter/execution-frame/execution-frame.h"

struct Trap : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct InternalCompilerError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Simulator {
    CompUnitIR compUnit;    
    std::unordered_map<std::string, int> nameToIndex;
    std::vector<int> memory;
    
    int heapSizeMax;

    ExprStack exprStack;
    std::unordered_set<std::string> libraryFunctions;

    int findLabel(std::string label);
protected:
    std::unordered_map<int, IR*> indexToNode;
    static const int debugLevel = 0;

    int getMemoryIndex(int addr);
    int libraryCall(std::string name, std::vector<int> args);
    void leave(ExecutionFrame& frame);

public:
    static const int DEFAULT_HEAP_SIZE = 2048;

    Simulator(CompUnitIR compUnit, int heapSizeMax = DEFAULT_HEAP_SIZE);
    Simulator(Simulator& other);

    int malloc(int size);
    int calloc(int size);
    int read(int addr);
    void store(int addr, int value);
    int call(std::string name, std::vector<int> args);
    int call(ExecutionFrame& parent, std::string name, std::vector<int> args);
};