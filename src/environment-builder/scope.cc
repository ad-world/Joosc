#include "scope.h"
#include "symboltable.h"
#include "exceptions/exceptions.h"

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
    declared_variables = {};
    open_scopes = {};
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

int LocalVariableScopeManager::getInsertPosition(const std::string &name) {
    int offset = 0;
    for (auto i = open_scopes.begin(); i != open_scopes.end(); ++i ) {
        auto table_result = scopes[(*i)].lookupUniqueSymbol(name);
        if (table_result) {
            return scopes[(*i)].getInsertPosition(name) + offset;
        }

        offset += scopes[(*i)].getSize();
    }
    
    return -1;
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

LocalVariableDeclarationObject* LocalVariableScopeManager::lookupDeclaredVariable(const std::string& name) {
    LocalVariableDeclarationObject* result = lookupVariable(name);
    if (result && declared_variables.find(result) != declared_variables.end()) {
        return result;
    }
    return nullptr;
}

void LocalVariableScopeManager::declareVariable(const std::string& name) {
    if (!lookupVariable(name) || lookupDeclaredVariable(name)) {
        THROW_CompilerError("declareVariable method called without correct conditions");
    }
    declared_variables.insert(lookupVariable(name));
}
