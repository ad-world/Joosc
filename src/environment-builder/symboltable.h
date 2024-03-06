#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <optional>
#include "symboltableentry.h"
#include "variant-ast/names.h"

class SymbolTable {
    std::unordered_map<std::string, std::list<SymbolTableEntry>> hashmap;
    std::unordered_map<std::string, int> insert_position;
    int current_insert_position = 0;
    
    // Compile time asserts that T is a member of SymbolTableEntry variant
    template <typename T>
    void assertTypeIsSymbolTableEntry() {
        static_assert(std::is_constructible<SymbolTableEntry, T>(), "Type must be SymbolTableEntry variant member");
    }
    
  public:
    SymbolTable() = default;

    // Lookup a symbol in the environment and return pointer to list of all matches if found
    // Return nullptr if no symbol found
    std::list<SymbolTableEntry>* lookupSymbol(const std::string& name);

    // Lookup a symbol in the environment and return pointer to just one match if found
    // If more than one match, the most recent match is returned
    // Return nullptr if no symbol found
    SymbolTableEntry* lookupUniqueSymbol(const std::string& name);

    // Get the insert order of a specific symbol table entry
    int getInsertPosition(const std::string &name);
    int getSize() { return hashmap.size(); };

    // Add new SymbolTableEntry corresponding to name
    // Returns pointer to SymbolTableEntry that was added
    //
    // T must be a member of SymbolTableEntry variant
    template <typename T>
    T* addSymbol(const std::string& name) {
        assertTypeIsSymbolTableEntry<T>();

        auto& matches = hashmap[name];
        matches.push_back(T(name));

        auto it = insert_position.find(name);

        if (it == insert_position.end()) {
            insert_position[name] = current_insert_position;
            current_insert_position++;
        }


        return &std::get<T>(matches.back());
    }

    // Call lookupUniqueSymbol<SymbolTableEntry*> and return a subtype
    template <typename T>
    T* lookupUniqueSymbol(const std::string& name) {
        assertTypeIsSymbolTableEntry<T>();
        SymbolTableEntry* result = lookupUniqueSymbol(name);
        if (result) {
            return &std::get<T>(*result);
        }
        return nullptr;
    }

};
