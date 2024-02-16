#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include "symboltableentry.h"
#include "variant-ast/names.h"

class SymbolTable {
    std::unordered_map<std::string, std::vector<SymbolTableEntry>> hashmap;
    
  public:
    SymbolTable();

    // Add symbol to this environment
    // Return true if symbol existed
    bool addSymbol(const std::string& name);

    // Lookup a symbol in the environment
    // Return nullopt if no symbol found
    std::optional<std::vector<SymbolTableEntry*>> lookupSymbol(const std::string& name);
};
