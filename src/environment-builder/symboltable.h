#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "symboltableentry.h"

class SymbolTable {
    SymbolTable* outer_scope_table;
    std::unordered_map<std::string, std::vector<SymbolTableEntry>> hashmap;
    
  public:
    SymbolTable();

    // Add symbol to this environment
    // Return true if symbol existed
    bool addSymbol(const std::string& name);

    // Lookup a symbol in the environment and parent environments
    // Return nullopt if no symbol found
    std::optional<std::vector<SymbolTableEntry>> lookupSymbol(const std::string& name);

    // Lookup a symbol in the current environment only, and not in the outer scopes
    // Return nullopt if no symbol found
    std::optional<std::vector<SymbolTableEntry>> lookupSymbolsLocally(const std::string& name);
};
