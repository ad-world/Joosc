#pragma once

#include "IR/ir_variant.h"
#include <memory>
#include <string>

class LabelIR {
    static size_t num_labels;
    std::string name;

  public:
    LabelIR(std::string name) : name{std::move(name)} {}

    std::string &getName() { return name; }
    static std::string generateName(std::string prefix = "") {
        num_labels++;
        return ( ( prefix.empty() ? "label" : prefix ) + std::to_string(num_labels) );
    }
    
    std::string label() { return "LABEL(" + name + ")"; }

    static std::unique_ptr<StatementIR> makeStmt(std::string str);
};
