#pragma once
#include <memory>
#include <vector>
#include <unordered_set>

struct LocalVariableDeclarationObject;
class SymbolTable;

// Manages the opened/closed scopes of a method declaration
class LocalVariableScopeManager {
    std::vector<SymbolTable> scopes;
    std::vector<size_t> open_scopes;
    // Tracks which variables have declared status
    std::unordered_set<LocalVariableDeclarationObject*> declared_variables;

  public:
    // Open scope if it exists. Does not check if scope is already open.
    // Return true if successful
    bool openScope(size_t scope_id);

    // Close scope if it exists and is open. Does not undeclare variables in the scope.
    // Return true if successful
    bool closeScope(size_t scope_id);

    // Close all currently open scopes. Undeclares all variables.
    void closeAllScopes();

    // Create a new scope and return its id. Does not open the scope on its own.
    size_t createNewScope();

    // Look up variable in the scope. Return nullptr if doesn't exist, otherwise return pointer to the variable
    LocalVariableDeclarationObject* lookupVariable(const std::string& name);

    void printScope();

    // Add variable to open scope. Return nullptr if already exists or no scope exists, 
    // otherwise return pointer to the variable
    LocalVariableDeclarationObject* addVariable(const std::string& name);

    int getInsertPosition(const std::string &name);

    // Calls lookupVariable and returns nullptr if the variable wasn't added, or wasn't "declared".
    // Otherwise, returns a pointer to the variable.
    LocalVariableDeclarationObject* lookupDeclaredVariable(const std::string& name);

    // Set variable in the current scope to have "declared" status. 
    // Must be called on a variable that is in the latest opened scope.
    void declareVariable(const std::string& name);
};
