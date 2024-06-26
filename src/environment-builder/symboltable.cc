#include <string>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include "symboltableentry.h"
#include "symboltable.h"
#include "variant-ast/names.h"

std::list<SymbolTableEntry>* SymbolTable::lookupSymbol(const std::string& name) {
    auto& matches = hashmap[name];

    if (matches.size() == 0) {
        return nullptr;
    } else {
        return &matches;
    }
}

SymbolTableEntry* SymbolTable::lookupUniqueSymbol(const std::string& name) {
    auto matches = lookupSymbol(name);

    if (matches) {
        return &matches->back();
    } else {
        return nullptr;
    }
}

int SymbolTable::getInsertPosition(const std::string &name) {
    auto &matches = hashmap[name];

    if(matches.size() == 0) {
        return -1;
    } else {
        return insert_position[name];
    }
}

