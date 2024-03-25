#include <stack>
#include <string>

struct StackItem;

class ExprStack {
    std::stack<StackItem> stack;

public:
    ExprStack() = default;
    int popValue();
    StackItem pop();
    void pushAddr(int value, int addr);
    void pushTemp(int value, int addr);
    void pushName(int value, std::string name);
    void pushValue(int value);
};