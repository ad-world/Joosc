#include <string>

class StackItem {
    enum Kind {
        COMPUTED,
        MEM,
        TEMP,
        NAME
    };

public:
    Kind type;
    int value;
    int addr;
    std::string temp;
    std::string name;

    StackItem(int value);
    StackItem(int value, int addr);
    StackItem(Kind type, int value, std::string string);
    
};