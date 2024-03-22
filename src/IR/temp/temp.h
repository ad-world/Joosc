#pragma once

#include <string>

class TempIR {
    static int num_temps;
    std::string name;

public:
    TempIR(std::string name) : name{std::move(name)} {}

    std::string &getName() { return name; }
    static std::string generateName() {
        num_temps++;
        return "t" + std::to_string(num_temps);
    }

    std::string label() { return "TEMP(" + name + ")"; }
    bool isConstant() { return false; }
};
