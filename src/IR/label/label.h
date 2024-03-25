#pragma once

#include <string>

class LabelIR {
    std::string name;

  public:
    LabelIR(std::string name) : name{std::move(name)} {}

    std::string &getName() { return name; }
    
    std::string label() { return "LABEL(" + name + ")"; }
};
