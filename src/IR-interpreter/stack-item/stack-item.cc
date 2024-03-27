#include "stack-item.h"

StackItem::StackItem(int value) : type(COMPUTED), value(value) {}
StackItem::StackItem(int value, int addr) : type(MEM), value(value), addr(addr) {}
StackItem::StackItem(Kind type, int value, std::string string) : type(type), value(value) {
    if (type == TEMP) {
        temp = string;
    } else if (type == NAME) {
        name = string;
    } else {
        throw "Unexpected type in StackItem constructor";
    }
}



