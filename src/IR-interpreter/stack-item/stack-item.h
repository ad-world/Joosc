#pragma once
#include <string>

class StackItem {
public:
    enum Kind {
        COMPUTED,
        MEM,
        TEMP,
        NAME
    };
    Kind type;
    int value;
    int addr;
    std::string temp;
    std::string name;

    StackItem(int value);
    StackItem(int value, int addr);
    StackItem(Kind type, int value, std::string string);
    std::string getKindString() {
        switch (type) {
            case Kind::COMPUTED:
                return "COMPUTED";
            case Kind::MEM:
                return "MEM";
            case Kind::TEMP:
                return "TEMP";
            case Kind::NAME:
                return "NAME";
            default:
                return "Unknown kind.";
        };
    }
};