#include "symboltableentry.h"
#include "symboltable.h"
#include "exceptions/exceptions.h"
#include <sstream>

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

PackageDeclarationObject* PackageDeclarationObject::findPackageDeclaration(std::vector<Identifier> &identifiers) {
    auto current = this;
    for (auto &identifier : identifiers) {
        auto result = current->sub_packages->lookupUniqueSymbol(identifier.name);
        if (result) {
            current = &std::get<PackageDeclarationObject>(*result);
        } else {
            return nullptr;
        }
    }
    return current;
}

ClassDeclarationObject* PackageDeclarationObject::findClassDeclaration(std::vector<Identifier> &identifiers) {
    if ( identifiers.size() == 0 ) { return nullptr; }
    auto current = this;
    
    std::string class_name = identifiers.back().name;

    auto prefix = identifiers; // copy
    prefix.pop_back();
    auto package = findPackageDeclaration(prefix);

    if(package) {
        auto result = package->classes->lookupUniqueSymbol(class_name);
        if (result) {
            return &std::get<ClassDeclarationObject>(*result);
        }
    }

    return nullptr;
}

InterfaceDeclarationObject* PackageDeclarationObject::findInterfaceDeclaration(std::vector<Identifier> &identifiers) {
    if ( identifiers.size() == 0 ) { return nullptr; }
    auto current = this;
    
    std::string interface_name = identifiers.back().name;

    auto prefix = identifiers; // copy
    prefix.pop_back();
    auto package = findPackageDeclaration(prefix);

    if(package) {
        auto result = package->interfaces->lookupUniqueSymbol(interface_name);
        if (result) {
            return &std::get<InterfaceDeclarationObject>(*result);
        }
    }

    return nullptr;
}

std::vector<Identifier> string_to_identifiers(std::string s) {
    std::istringstream ss(s);
    std::vector<Identifier> tokens;
    for ( std::string each; std::getline(ss, each, '.'); tokens.push_back(Identifier(each)) );

    return tokens;
}

PackageDeclarationObject* PackageDeclarationObject::findPackageDeclaration(std::string str) {
    std::vector<Identifier> ids = string_to_identifiers(str);
    return findPackageDeclaration(ids);
}

ClassDeclarationObject* PackageDeclarationObject::findClassDeclaration(std::string str) {
    std::vector<Identifier> ids = string_to_identifiers(str);
    return findClassDeclaration(ids);
}
InterfaceDeclarationObject* PackageDeclarationObject::findInterfaceDeclaration(std::string str) {
    std::vector<Identifier> ids = string_to_identifiers(str);
    return findInterfaceDeclaration(ids);
}

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

// Helpers

ClassDeclarationObject* PackageDeclarationObject::getJavaLangObject() {
    ClassDeclarationObject* object_class = nullptr;
    try {
        auto java_package_variant = sub_packages->lookupUniqueSymbol("java");
        auto& java_package = std::get<PackageDeclarationObject>(*java_package_variant);
        auto lang_package_variant = java_package.sub_packages->lookupUniqueSymbol("lang");
        auto& lang_package = std::get<PackageDeclarationObject>(*lang_package_variant);
        auto object_class_variant = lang_package.classes->lookupUniqueSymbol("Object");
        object_class = &std::get<ClassDeclarationObject>(*object_class_variant);
    } catch (...) {
        // Error getting java.lang.Object
        THROW_TypeLinkerError("java.lang.Object not found");
    }
    return object_class;
}
