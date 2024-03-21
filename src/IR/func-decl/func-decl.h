#pragma once

#include <string>
#include <memory>
#include "IR/ir.h"

class FuncDeclIR {
    std::string name;
    std::unique_ptr<StatementIR> body;
    int num_params;

  public:
    FuncDeclIR(std::string name, std::unique_ptr<StatementIR> body, int num_params);
        // : name{std::move(name)}, body{std::move(body)}, num_params{num_params} {}

    std::string &getName() { return name; }
    StatementIR &getBody() { return *body.get(); }
    int getNumParams() { return num_params; }

    std::string label() { return "FUNC " + name; }
};
