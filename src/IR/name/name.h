#pragma once

#include <string>

class NameIR {
    std::string name; 

public:
    NameIR(std::string name) : name{std::move(name)} {}
    std::string &getName() { return name; }
    std::string label() { return "NAME(" + name + ")"; }
    bool isConstant() { return false; }
};