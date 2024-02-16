#include "scope.h"
#include "symboltable.h"

bool LocalVariableScopeManager::openScope(size_t scope_id) {
    if (scope_id < scopes.size()) {
        open_scopes.push_back(scope_id);
        return true;
    }
    return false;
}

bool LocalVariableScopeManager::closeScope(size_t scope_id) {
    if (scope_id < scopes.size() && open_scopes.back() == scope_id) {
        open_scopes.pop_back();
        return true;
    }
    return false;
}

void LocalVariableScopeManager::closeAllScopes() {
    open_scopes = std::vector<size_t>();
}

size_t LocalVariableScopeManager::createNewScope() {
    scopes.emplace_back(SymbolTable());
    return scopes.size() - 1;
}

LocalVariableDeclarationObject* LocalVariableScopeManager::lookupVariable(const std::string& name) {
    for (auto i = open_scopes.rbegin(); i != open_scopes.rend(); ++i ) {
        auto table_result = scopes[(*i)].lookupUniqueSymbol(name);
        if (table_result) {
            return &std::get<LocalVariableDeclarationObject>(*table_result);
        }
    }
    return nullptr;
}

LocalVariableDeclarationObject* LocalVariableScopeManager::addVariable(const std::string& name) {
    if (lookupVariable(name)) {
        return nullptr;
    }
    if (open_scopes.empty()) {
        return nullptr;
    }
    return scopes[open_scopes.back()].addSymbol<LocalVariableDeclarationObject>(name);
}
