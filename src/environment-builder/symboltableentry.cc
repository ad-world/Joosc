#include "symboltableentry.h"
#include "symboltable.h"
#include "exceptions/exceptions.h"

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
