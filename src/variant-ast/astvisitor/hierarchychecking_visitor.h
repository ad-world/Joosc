#pragma once

#include <map>
#include <stdexcept>
#include <unordered_set>
#include "variant-ast/astnode.h"
#include "defaultskipvisitor.h"
#include "environment-builder/symboltable.h"
#include "variant-ast/classes.h"
#include "variant-ast/names.h"
#include "variant-ast/types.h"
#include <functional>
#include <variant>

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
        default:
            throw std::runtime_error("Unrecognized primitive type");
    }
}

std::string getFormalParameterType(const FormalParameter& formalParameter) {
    std::string formalParameterType = "";
    if(std::get_if<QualifiedIdentifier>(&(*formalParameter.type->non_array_type))) {
        auto typeDecl = formalParameter.type->node;
        if(std::get_if<ClassDeclarationObject*>(&typeDecl)) {
            formalParameterType += std::get<ClassDeclarationObject*>(typeDecl)->identifier;
        } else {
            formalParameterType += std::get<InterfaceDeclarationObject*>(typeDecl)->identifier;
        }
    } else {
        formalParameterType += PrimitiveTypeToString(std::get<PrimitiveType>(*formalParameter.type->non_array_type));
    }
    if(formalParameter.type->is_array) {
        formalParameterType += "[]";
    }
    return formalParameterType;
}

std::string getMethodSignature(const MethodDeclaration& method) {
    std::string methodSignature = method.function_name->name + "(";
    for(const auto& formalParameter: method.parameters) {
        methodSignature += getFormalParameterType(formalParameter);
        methodSignature += ",";
    }
    methodSignature.pop_back();
    methodSignature += ")";
    return methodSignature;
}

// Return true if they have the same signature
bool operator==(const MethodDeclaration& lhs, const MethodDeclaration& rhs) {
    return getMethodSignature(lhs) == getMethodSignature(rhs);
}

bool checkMethodWithSameSignature(std::vector<MethodDeclaration>& methods) {
    std::unordered_set<std::string> methodSignatureSet{};
    for(const auto& method: methods) {
        std::string methodSignature = getMethodSignature(method);
        methodSignatureSet.insert(methodSignature);
    }
    return methods.size() != methodSignatureSet.size();
}


bool checkCyclicHierarchy(std::variant<ClassDeclarationObject*, InterfaceDeclarationObject*> obj) {
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

    if ( std::holds_alternative<ClassDeclarationObject*>(obj) ) {
        // Classdecl
        return dfsClass(std::get<ClassDeclarationObject*>(obj));
    } else {
        // Interfacedecl
        return dfsInterface(std::get<InterfaceDeclarationObject*>(obj));
    }
}

bool operator==(const Type & lhs, const Type & rhs) {
    if ( lhs.is_array != rhs.is_array ) {
        return false;
    }

    if ( std::holds_alternative<QualifiedIdentifier>(*lhs.non_array_type) && std::holds_alternative<QualifiedIdentifier>(*rhs.non_array_type) ) {
        return (
            QualifiedIdentifierToString(std::get<QualifiedIdentifier>(*lhs.non_array_type))
            == QualifiedIdentifierToString(std::get<QualifiedIdentifier>(*rhs.non_array_type))
        );
    } else if ( std::holds_alternative<PrimitiveType>(*lhs.non_array_type) && std::holds_alternative<PrimitiveType>(*rhs.non_array_type) ) {
        return (
            std::get<PrimitiveType>(*lhs.non_array_type)
            == std::get<PrimitiveType>(*rhs.non_array_type)
        );
    } else {
        return false;
    }
}

bool operator!=(const Type & lhs, const Type & rhs) {
    return !(lhs == rhs);
}

// 3. (JLS 8.1.1.1, 8.4, 8.4.2, 8.4.6.3, 8.4.6.4, 9.2, 9.4.1)
void checkMethodReplaceReturnType(const MethodDeclaration& method, std::vector<MethodDeclaration*>& replaced_methods) {
    for  ( const auto & extend_method : replaced_methods ) {
        if (method.type != extend_method->type) {
            throw std::runtime_error("Hierarchy checking: overriding same method signature, different return type.");
        }
    }
}

// 1. (JLS 8.4.6.1)
// 2. (JLS 8.4.6.2)
// 4. (JLS 8.4.6.3)
// 5. (JLS 8.4.3.3)
void checkMethodReplaceModifiers(
    const MethodDeclaration &method,
    std::vector<MethodDeclaration*> &replaced_methods,
    std::unordered_set<Modifier> &method_modifiers,
    std::unordered_set<Modifier> &replaced_modifiers,
    std::unordered_map<MethodDeclaration*, std::unordered_set<Modifier>> &replaced_modifiers_map
) {
    if ( method_modifiers.find(Modifier::STATIC) != method_modifiers.end() ) {
        // static method
        for ( const auto & extend_method : replaced_methods ) {
            std::unordered_set<Modifier> *extend_modifiers = &replaced_modifiers_map.at(extend_method);
            if ( extend_modifiers->find(Modifier::STATIC) == extend_modifiers->end() ) {
                // replaced non-static
                throw std::runtime_error("Hierarchy checking: A static method must not replace a nonstatic method.");
            }
        }
    } else {
        // non-static method
        if ( replaced_modifiers.find(Modifier::STATIC) != replaced_modifiers.end() ) {
            // replaced static
            throw std::runtime_error("Hierarchy checking: A nonstatic method must not replace a static method.");
        }
    }

    if ( method_modifiers.find(Modifier::PROTECTED) != method_modifiers.end() ) {
        // protected method
        if ( replaced_modifiers.find(Modifier::PUBLIC) != replaced_modifiers.end() ) {
            // replaced public
            throw std::runtime_error("Hierarchy checking: A protected method must not replace a public method.");
        }
    }

    if ( replaced_modifiers.find(Modifier::FINAL) != replaced_modifiers.end() ) {
        // replaced final
        throw std::runtime_error("Hierarchy checking: A method must not replace a final method.");
    }
}

void checkMethods(std::vector<MethodDeclaration>& methods, ClassDeclaration& parent_class) {
    for ( const auto& method : methods ) {
        std::unordered_set<Modifier> modifier_set;
        for ( const auto& modifier : method.modifiers ) {
            modifier_set.insert(modifier);
        }

        // Get all replacable methods
        std::vector<MethodDeclaration*> replaced_methods;
        std::unordered_set<Modifier> replaced_modifiers;
        std::unordered_map<MethodDeclaration*, std::unordered_set<Modifier>> replaced_modifiers_map;
        for (
            auto& classEnv = parent_class.environment;
            classEnv->extended != nullptr;
            classEnv = classEnv->extended
        ) {
            for ( auto& extend_method : classEnv->ast_reference->method_declarations ) {
                if ( method == extend_method ) {
                    replaced_methods.push_back(&extend_method);
                    std::unordered_set<Modifier> extend_modifiers;
                    for ( const auto& modifier : extend_method.modifiers ) {
                        replaced_modifiers.insert(modifier);
                        extend_modifiers.insert(modifier);
                    }
                    replaced_modifiers_map.insert({&extend_method, extend_modifiers});
                }
            }
        }

        checkMethodReplaceReturnType(method, replaced_methods);
        checkMethodReplaceModifiers(method, replaced_methods, modifier_set, replaced_modifiers, replaced_modifiers_map);
    }
}

// Get all methods contained (declared or inherited) in a class
std::vector<MethodDeclarationObject*> getAllMethods(ClassDeclarationObject* classObj) {
    std::vector<MethodDeclarationObject*> allMethods{};
    std::function<void(ClassDeclarationObject*)> dfsClass; // Declare dfsClass lambda function
    std::function<void(InterfaceDeclarationObject*)> dfsInterface; // Declare dfsInterface lambda function

    dfsClass = [&](ClassDeclarationObject* currentClassObj) {
        for (auto& method : classObj->ast_reference->method_declarations) {
            allMethods.push_back(method.environment);
        }
        if (currentClassObj->extended != nullptr) {
            dfsClass(currentClassObj->extended);
        }
        for(auto& implementedInterface : currentClassObj->implemented) {
            dfsInterface(implementedInterface);
        }
    };
    dfsInterface = [&](InterfaceDeclarationObject* currentInterfaceObj) {
        for (auto& method : currentInterfaceObj->ast_reference->method_declarations) {
            allMethods.push_back(method.environment);
        }
        for(auto& extendedInterface : currentInterfaceObj->extended) {
            dfsInterface(extendedInterface);
        }
    };
    dfsClass(classObj);
    return allMethods;
}

class HierarchyCheckingVisitor : public DefaultSkipVisitor<void> {
    PackageDeclarationObject* root_symbol_table;
    // Environment* root_env;
    
public:
    using DefaultSkipVisitor<void>::operator();
    void operator()(ClassDeclaration &node) override {
        auto &extends = node.extends_class;
        auto &classEnv = node.environment;
        auto &extendedClass = classEnv->extended;
        std::string className = node.class_name->name;

        // A class must not extend a final class. (JLS 8.1.1.2, 8.1.3)
        // Check that extends is NOT final
        if ( extendedClass != nullptr ) {
            auto extendedClassModifiers = extendedClass->ast_reference->modifiers;
            for(const auto &modifier : extendedClassModifiers) {
                if(modifier == Modifier::FINAL) {
                    throw std::runtime_error("Error: Class extends final class.");
                }
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
            throw std::runtime_error("Error: Class repeats an interface in its implements clause");
        }        

        // A class must not declare two constructors with the same parameter types (JLS 8.8.2)
        // A class or interface must not declare two methods with the same signature (name and parameter types). (JLS 8.4, 9.4)
        std::vector<MethodDeclaration>& methods = node.method_declarations;
        // Check that no two methods have the same name and parameter types
        if(checkMethodWithSameSignature(methods)) {
            throw std::runtime_error("Error: Class declares two methods with the same signature");
        }

        // The hierarchy must be acyclic. (JLS 8.1.3, 9.1.2)
        if(checkCyclicHierarchy(classEnv)) {
            throw std::runtime_error("Error: Class has a cyclic hierarchy");
        }

        // A class that contains (declares or inherits) any abstract methods must be abstract. (JLS 8.1.1.1)
        // get all the methods from the class (declared or inherited) and check if any of them are abstract
        std::vector<MethodDeclarationObject*> allMethods{};
        if(std::find(node.modifiers.begin(), node.modifiers.end(), Modifier::ABSTRACT) == node.modifiers.end()) {
            allMethods = getAllMethods(classEnv);
            for(auto& method: allMethods) {
                if(std::find(method->ast_reference->modifiers.begin(), method->ast_reference->modifiers.end(), Modifier::ABSTRACT) != method->ast_reference->modifiers.end()) {
                    throw std::runtime_error("Error: Class contains abstract method but is not abstract");
                }
            }
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
            throw std::runtime_error("Error: Interface repeats an interface in its extends clause");
        }

        // A class must not declare two constructors with the same parameter types (JLS 8.8.2)
        // A class or interface must not declare two methods with the same signature (name and parameter types). (JLS 8.4, 9.4)
        std::vector<MethodDeclaration>& methods = node.method_declarations;
        // Check that no two methods have the same name and parameter types
        if(checkMethodWithSameSignature(methods)) {
            throw std::runtime_error("Error: Interface declares two methods with the same signature");
        }

        // The hierarchy must be acyclic. (JLS 8.1.3, 9.1.2)
        if(checkCyclicHierarchy(interfaceEnv)) {
            throw std::runtime_error("Error: Interface has a cyclic hierarchy");
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

    void visit(AstNodeVariant &node) override {
        std::visit(*this, node);
    }

    HierarchyCheckingVisitor(PackageDeclarationObject &root_symbol_table) : root_symbol_table{&root_symbol_table} {};
        // HierarchyCheckingVisitor(Environment *root_env) : root_env{std::move(root_env)} {};
};
