#include <unordered_map>
#include "IR/ir.h"
#include <string>

class ExecutionFrame {
    std::unordered_map<std::string, int> regs;
protected:  
    int ip;
    int ret;

public:
    ExecutionFrame(int ip);
    int get(std::string tempName);
    void put(std::string tempName, int value);
    bool advance();
    void setIP(int ip);
    IR* getCurrentNode();    
};