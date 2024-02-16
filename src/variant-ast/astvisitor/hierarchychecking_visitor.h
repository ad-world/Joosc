#pragma once

#include <iostream>
#include <unordered_set>
#include "variant-ast/astnode.h"
#include "defaultskipvisitor.h"
#include "symboltable.h"
#include <functional>

std::string QualifiedIdentifierToString(QualifiedIdentifier& qi) {
    std::string result = "";
    for(const auto& identifier: qi.identifiers) {
        result += identifier.name + ".";
    }
    result.pop_back();
    return result;
}

std::string PrimitiveTypeToString(PrimitiveType& pt) {
    switch(pt) {
        case PrimitiveType::BYTE:
            return "byte";
        case PrimitiveType::SHORT:
            return "short";
        case PrimitiveType::INT:
            return "int";
        case PrimitiveType::CHAR:
            return "char";
        case PrimitiveType::BOOLEAN:
            return "boolean";
        case PrimitiveType::VOID:
            return "void";
    }
}

bool checkMethodWithSameSignature(std::vector<MethodDeclaration>& methods) {
    std::unordered_set<std::string> methodSignatureSet{};
    for(const auto& method: methods) {
        std::string methodSignature = method.function_name->name + "(";
        for(const auto& formalParameter: method.parameters) {
            if(std::get_if<QualifiedIdentifier>(&(*formalParameter.type->non_array_type))) {
                methodSignature += QualifiedIdentifierToString(std::get<QualifiedIdentifier>(*formalParameter.type->non_array_type));
            } else {
                methodSignature += PrimitiveTypeToString(std::get<PrimitiveType>(*formalParameter.type->non_array_type));
            }
            if(formalParameter.type->is_array) {
                methodSignature += "[]";
            }
            methodSignature += ",";
        }
        methodSignature.pop_back();
        methodSignature += ")";
        methodSignatureSet.insert(methodSignature);
    }
    return methods.size() != methodSignatureSet.size();
}

template <typename T>
bool checkCyclicHierarchy(T* obj) {
    std::unordered_set<ClassDeclarationObject*> visitedClasses{};
    std::unordered_set<InterfaceDeclarationObject*> visitedInterfaces{};
    std::unordered_set<ClassDeclarationObject*> visitingClasses{};
    std::unordered_set<InterfaceDeclarationObject*> visitingInterfaces{};

    // Lambda for checking classes
    std::function<bool(ClassDeclarationObject*)> dfsClass;
    // Lambda for checking interfaces
    std::function<bool(InterfaceDeclarationObject*)> dfsInterface;

    dfsClass = [&](ClassDeclarationObject* currentClassObj) -> bool {
        if (visitedClasses.find(currentClassObj) != visitedClasses.end()) {
            return false;
        }
        if (visitingClasses.find(currentClassObj) != visitingClasses.end()) {
            return true;
        }
        visitingClasses.insert(currentClassObj);

        if (currentClassObj->extended != nullptr && dfsClass(currentClassObj->extended)) {
            return true;
        }

        for (auto& implementedInterface : currentClassObj->implemented) {
            if (implementedInterface != nullptr && dfsInterface(implementedInterface)) {
                return true;
            }
        }

        visitingClasses.erase(currentClassObj);
        visitedClasses.insert(currentClassObj);
        return false;
    };

    dfsInterface = [&](InterfaceDeclarationObject* currentInterfaceObj) -> bool {
        if (visitedInterfaces.find(currentInterfaceObj) != visitedInterfaces.end()) {
            return false;
        }
        if (visitingInterfaces.find(currentInterfaceObj) != visitingInterfaces.end()) {
            return true;
        }
        visitingInterfaces.insert(currentInterfaceObj);

        for (auto& extendedInterface : currentInterfaceObj->extended) {
            if (extendedInterface != nullptr && dfsInterface(extendedInterface)) {
                return true;
            }
        }

        visitingInterfaces.erase(currentInterfaceObj);
        visitedInterfaces.insert(currentInterfaceObj);
        return false;
    };

    return dfsClass(obj);
}

class HierarchyCheckingVisitor : public DefaultSkipVisitor<void> {
    PackageDeclarationObject* root_symbol_table;
    
    using DefaultSkipVisitor<void>::operator();
    void operator()(ClassDeclaration &node) override {
        auto &extends = node.extends_class;
        auto &classEnv = node.environment;
        auto &extendedClass = classEnv->extended;
        std::string className = node.class_name->name;
        
        // A class must not extend a final class. (JLS 8.1.1.2, 8.1.3)
        // Check that extends is NOT final
        auto extendedClassModifiers = extendedClass->ast_reference->modifiers;
        for(const auto &modifier : extendedClassModifiers) {
            if(modifier == Modifier::FINAL) {
                std::cerr << "Error: Class " << className << " extends final class " << extendedClass->identifier << std::endl;
            }
        }

        // An interface must not be repeated in an implements clause (JLS 8.1.4)
        auto &implements = node.implements;
        std::unordered_set<std::string> implementsSet{};
        for(const auto& qi: implements) {
            std::string interfaceName = "";
            for(const auto& identifierVec: qi.identifiers) {
                interfaceName += identifierVec.name;
            }
            implementsSet.insert(interfaceName);
        }
        if(implements.size() != implementsSet.size()) {
            std::cerr << "Error: Class " << node.class_name->name << " repeats an interface in its implements clause" << std::endl;
        }        

        // A class must not declare two constructors with the same parameter types (JLS 8.8.2)
        // A class or interface must not declare two methods with the same signature (name and parameter types). (JLS 8.4, 9.4)
        std::vector<MethodDeclaration>& methods = node.method_declarations;
        // Check that no two methods have the same name and parameter types
        if(checkMethodWithSameSignature(methods)) {
            std::cerr << "Error: Class " << node.class_name->name << " declares two methods with the same signature" << std::endl;
        }

        // The hierarchy must be acyclic. (JLS 8.1.3, 9.1.2)
        if(checkCyclicHierarchy(classEnv)) {
            std::cerr << "Error: Class " << node.class_name->name << " has a cyclic hierarchy" << std::endl;
        }

        this->visit_children(node);
    };

    void operator()(InterfaceDeclaration &node) override {
        // An interface must not be repeated in an extends clause (JLS 8.1.4)
        auto& interfaceEnv= node.environment;
        auto& extended = interfaceEnv->extended;
        std::unordered_set<std::string> extendedSet{};
        for(const auto& identifier: extended) {
            std::string interfaceName = "";
            extendedSet.insert(interfaceName);
        }
        if(extended.size() != extendedSet.size()) {
            std::cerr << "Error: Interface " << node.interface_name->name << " repeats an interface in its extends clause" << std::endl;
        }

        // A class must not declare two constructors with the same parameter types (JLS 8.8.2)
        // A class or interface must not declare two methods with the same signature (name and parameter types). (JLS 8.4, 9.4)
        std::vector<MethodDeclaration>& methods = node.method_declarations;
        // Check that no two methods have the same name and parameter types
        if(checkMethodWithSameSignature(methods)) {
            std::cerr << "Error: Interface " << node.interface_name->name << " declares two methods with the same signature" << std::endl;
        }

        // The hierarchy must be acyclic. (JLS 8.1.3, 9.1.2)
        if(checkCyclicHierarchy(interfaceEnv)) {
            std::cerr << "Error: Interface " << node.interface_name->name << " has a cyclic hierarchy" << std::endl;
        }

        this->visit_children(node);
    }

    void operator()(MethodDeclaration &node) override {
        // A class that contains (declares or inherits) any abstract methods must be abstract. (JLS 8.1.1.1)
        auto &modifiers = node.modifiers;

        // If modifiers includes ABSTRACT, then class must be abstract as well
        auto abstractIt = std::find(modifiers.begin(), modifiers.end(), Modifier::ABSTRACT);
        
        if(abstractIt != modifiers.end()) {
            // Check that class is abstract
            // i.e. get parent of the Method, check that parent is a class, and that class is abstract
        }

        // A nonstatic method must not replace a static method. (JLS 8.4.6.1)
        // A static method must not replace a nonstatic method. (JLS 8.4.6.2)
        // A method must not replace a method with a different return type. (JLS 8.1.1.1, 8.4, 8.4.2, 8.4.6.3, 8.4.6.4, 9.2, 9.4.1)
        // A protected method must not replace a public method. (JLS 8.4.6.3)
        // A method must not replace a final method. (JLS 8.4.3.3)

        this->visit_children(node);
    }

    public:
        HierarchyCheckingVisitor(PackageDeclarationObject* root_symbol_table) : root_symbol_table{std::move(root_symbol_table)} {};
};