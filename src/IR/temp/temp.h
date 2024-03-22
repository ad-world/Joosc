#pragma once

#include <string>

class TempIR {
    static int num_temps;
    std::string name;

    public:
        TempIR() { num_temps++; name = "t" + std::to_string(num_temps); }
        TempIR(std::string name) : name{std::move(name)} { num_temps++; }

        std::string &getName() { return name; }
        
        std::string label() { return "TEMP(" + name + ")"; }
        bool isConstant() { return false; }
};
