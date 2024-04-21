#pragma once

#include <string>
#include <memory>
#include "IR/ir_variant.h"

class CommentIR {
    std::string text;
  public:
    explicit CommentIR(std::string text) : text{std::move(text)} {}

    std::string &getText() { return text; }

    std::string label() { return "COMMENT(" + text + ")"; }

    static std::unique_ptr<StatementIR> makeStmt(std::string text);
};
