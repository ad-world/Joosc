#include "symboltableentry.h"
#include "symboltable.h"

PackageDeclarationObject::PackageDeclarationObject(const std::string &identifier) : 
    identifier{identifier}, is_default_package{false} {}

PackageDeclarationObject::PackageDeclarationObject() : 
    identifier{""}, is_default_package{true} {}

ClassDeclarationObject::ClassDeclarationObject(const std::string &identifier) :
    identifier{identifier}, fields{std::make_unique<SymbolTable>()}, methods{} {}

InterfaceDeclarationObject::InterfaceDeclarationObject(const std::string &identifier) :
    identifier{identifier} {}

FieldDeclarationObject::FieldDeclarationObject(const std::string &identifier) :
    identifier{identifier} {}

MethodDeclarationObject::MethodDeclarationObject(const std::string &identifier) :
    identifier{identifier} {}

FormalParameterDeclarationObject::FormalParameterDeclarationObject(const std::string &identifier) :
    identifier{identifier} {}

LocalVariableDeclarationObject::LocalVariableDeclarationObject(const std::string &identifier) :
    identifier{identifier} {}
