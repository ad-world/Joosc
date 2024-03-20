#pragma once

#include <string>

class TempIR {
    std::string name;

    public:
        TempIR(std::string name) : name{std::move(name)} {}

        std::string &name() { return name; }
        
        std::string label() { return "TEMP(" + name + ")"; }
        bool isConstant() { return false; }
};
