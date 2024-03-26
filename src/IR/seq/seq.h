#pragma once

#include "IR/ir_variant.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <iterator>

class SeqIR {
    std::vector<std::unique_ptr<StatementIR>> stmts;
    bool replaceParent = false;

    static void filterNulls(std::vector<std::unique_ptr<StatementIR>>& list) {
        list.erase(std::remove_if(list.begin(), list.end(), [](const std::unique_ptr<StatementIR>& ptr) { return ptr.get() == nullptr; }), list.end());
    }

public:
    SeqIR(std::vector<std::unique_ptr<StatementIR>> stmts, bool replaceParent = false) {
        filterNulls(stmts);
        this->stmts = std::move(stmts);
        this->replaceParent = replaceParent;
    }

    std::vector<std::unique_ptr<StatementIR>> &getStmts() { return stmts; }

    std::string label() { return "SEQ"; }

    static std::unique_ptr<StatementIR> makeEmpty();
    static std::unique_ptr<StatementIR> makeStmt(std::vector<std::unique_ptr<StatementIR>> stmts, bool replaceParent = false);
};
