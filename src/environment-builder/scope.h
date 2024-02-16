#pragma once
#include <memory>
#include <vector>

struct LocalVariableDeclarationObject;
class SymbolTable;

// Manages the opened/closed scopes of a method declaration
class LocalVariableScopeManager {
    std::vector<SymbolTable> scopes;
    std::vector<size_t> open_scopes;

  public:
    // Open scope if it exists. Does not check if scope is already open.
    // Return true if successful
    bool openScope(size_t scope_id);

    // Close scope if it exists and is open
    // Return true if successful
    bool closeScope(size_t scope_id);

    // Close all currently open scopes
    void closeAllScopes();

    // Create a new scope and return its id. Does not open the scope on its own.
    size_t createNewScope();

    // Look up variable in the scope. Return nullptr if doesn't exist, otherwise return pointer to the variable
    LocalVariableDeclarationObject* lookupVariable(const std::string& name);

    // Add variable to open scope. Return nullptr if already exists or no scope exists, 
    // otherwise return pointer to the variable
    LocalVariableDeclarationObject* addVariable(const std::string& name);
};
