#pragma once

#include <string>

class TempIR {
    static size_t num_temps;
    std::string name;

public:
    TempIR(std::string name) : name{std::move(name)} {}

    std::string &getName() { return name; }
    static std::string generateName() {
        num_temps++;
        return "temp" + std::to_string(num_temps);
    }

    std::string label() { return "TEMP(" + name + ")"; }
    bool isConstant() { return false; }
};
