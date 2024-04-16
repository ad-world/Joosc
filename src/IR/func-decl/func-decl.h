#pragma once

#include <string>
#include <memory>
#include "IR/ir_variant.h"
#include <cassert>

class FuncDeclIR {
    std::string name;
    std::unique_ptr<StatementIR> body;
    int num_params;

  public:
    FuncDeclIR(std::string name, std::unique_ptr<StatementIR> body, int num_params)
        : name{name}, body{std::move(body)}, num_params{num_params} {}

    std::string &getName() { return name; }
    StatementIR &getBody() { assert(body.get()); return *body.get(); }
    void setBody(std::unique_ptr<StatementIR> other) { body = std::move(other); }
    int getNumParams() { return num_params; }

    std::string label() { return "FUNC " + name; }
};
