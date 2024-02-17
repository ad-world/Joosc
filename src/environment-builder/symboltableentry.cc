#include "symboltableentry.h"
#include "symboltable.h"

std::unique_ptr<SymbolTable> init_table() {
    return std::make_unique<SymbolTable>();
}

PackageDeclarationObject::PackageDeclarationObject(const std::string &identifier) : 
    identifier{identifier},
    is_default_package{false},
    sub_packages{init_table()},
    classes{init_table()},
    interfaces{init_table()} {}

PackageDeclarationObject::PackageDeclarationObject() : 
    identifier{""},
    is_default_package{true},
    sub_packages{init_table()},
    classes{init_table()},
    interfaces{init_table()} {}

ClassDeclarationObject::ClassDeclarationObject(const std::string &identifier) :
    identifier{identifier}, fields{init_table()}, methods{init_table()} {}

InterfaceDeclarationObject::InterfaceDeclarationObject(const std::string &identifier) :
    identifier{identifier}, methods{init_table()} {}

FieldDeclarationObject::FieldDeclarationObject(const std::string &identifier) :
    identifier{identifier} {}

MethodDeclarationObject::MethodDeclarationObject(const std::string &identifier) :
    identifier{identifier}, parameters{init_table()} {}

FormalParameterDeclarationObject::FormalParameterDeclarationObject(const std::string &identifier) :
    identifier{identifier} {}

LocalVariableDeclarationObject::LocalVariableDeclarationObject(const std::string &identifier) :
    identifier{identifier} {}
