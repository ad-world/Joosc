#include "symboltableentry.h"
#include "symboltable.h"

std::unique_ptr<SymbolTable> init_table() {
    return std::make_unique<SymbolTable>();
}

std::optional<TypeDeclaration> PackageDeclarationObject::tryFindTypeInPackage(std::string &identifier) {
    auto possible_classes = this->classes->lookupSymbol(identifier);
    auto possible_interfaces = this->interfaces->lookupSymbol(identifier);

    if (possible_classes) {
        return &std::get<ClassDeclarationObject>(possible_classes->back());
    } else if (possible_interfaces) {
        return &std::get<InterfaceDeclarationObject>(possible_interfaces->back());
    }
    return std::nullopt;
}

std::optional<PackageDeclarationObject*> PackageDeclarationObject::tryFindPackageInPackage(
    QualifiedIdentifier &qualified_identifier
) {
    PackageDeclarationObject* temp_package = this;

    for (auto &identifier : qualified_identifier.identifiers) {
        // Resolve to subpackage
        auto possible_package = temp_package->sub_packages->lookupUniqueSymbol(identifier.name);
        if (possible_package) {
            temp_package = &(std::get<PackageDeclarationObject>(*possible_package));
        } else {
            // Cannot find package
            return std::nullopt;
        }
    }

    return temp_package;
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
