#pragma once

#include <map>
#include <stdexcept>
#include <unordered_set>
#include "exceptions/exceptions.h"
#include "variant-ast/astnode.h"
#include "defaultskipvisitor.h"
#include "environment-builder/symboltable.h"
#include "variant-ast/classes.h"
#include "variant-ast/names.h"
#include "variant-ast/types.h"
#include <functional>
#include <variant>
#include <queue>
#include <iostream>

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
        auto typeDecl = formalParameter.type->link.linked_type;
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
    bool first = true;
    for(const auto& formalParameter: method.parameters) {
        if ( first ) { methodSignature += ","; first = false; }
        methodSignature += getFormalParameterType(formalParameter);
    }
    methodSignature += ")";
    return methodSignature;
}

// Return true if they have the same signature
bool operator==(const MethodDeclaration& lhs, const MethodDeclaration& rhs) {
    return getMethodSignature(lhs) == getMethodSignature(rhs);
}

bool checkMethodWithSameSignature(std::vector<MethodDeclaration*>& methods) {
    std::unordered_set<std::string> methodSignatureSet{};
    for(const auto& method: methods) {
        std::string methodSignature = getMethodSignature(*method);
        if ( methodSignatureSet.insert(methodSignature).second == false ) {
            // Method signature already exists
            return true;
        }
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

// 1. (JLS 8.4.6.1)
// 2. (JLS 8.4.6.2)
// 3. (JLS 8.1.1.1, 8.4, 8.4.2, 8.4.6.3, 8.4.6.4, 9.2, 9.4.1)
// 4. (JLS 8.4.6.3)
// 5. (JLS 8.4.3.3)
void checkMethodReplacement( MethodDeclaration& method, MethodDeclaration& replaced ) {
    if ( *method.type != *replaced.type ) {
        THROW_HierarchyError("A method must not replace a method with a different return type.");
    }

    if ( method.hasModifier(Modifier::STATIC) && (! replaced.hasModifier(Modifier::STATIC)) ) {
        THROW_HierarchyError("A static method must not replace a nonstatic method.");
    }

    if ( (! method.hasModifier(Modifier::STATIC)) && replaced.hasModifier(Modifier::STATIC)) {
        THROW_HierarchyError("A nonstatic method must not replace a static method.");
    }

    if ( method.hasModifier(Modifier::PROTECTED) && replaced.hasModifier(Modifier::PUBLIC)) {
        THROW_HierarchyError("A protected method must not replace a public method.");
    }

    if ( replaced.hasModifier(Modifier::FINAL) ) {
        THROW_HierarchyError("A method must not replace a final method.");
    }
}

// Same function as below, without dfsClass
std::vector<MethodDeclarationObject*> getAllMethods(InterfaceDeclarationObject* interfaceObj) {
    std::vector<MethodDeclarationObject*> allMethods{};
    std::function<void(InterfaceDeclarationObject*)> dfsInterface; // Declare dfsInterface lambda function

    dfsInterface = [&](InterfaceDeclarationObject* currentInterfaceObj) {
        for (auto& method : currentInterfaceObj->ast_reference->method_declarations) {
            // TODO: ensure this is not a constructor
            if ( ! method.environment->is_constructor ) {
                allMethods.push_back(method.environment);
            }
        }
        for(auto& extendedInterface : currentInterfaceObj->extended) {
            dfsInterface(extendedInterface);
        }
    };

    dfsInterface(interfaceObj);
    return allMethods;
}

// Get all methods contained (declared or inherited) in a class
std::vector<MethodDeclarationObject*> getAllMethods(ClassDeclarationObject* classObj) {
    std::vector<MethodDeclarationObject*> allMethods{};
    std::function<void(ClassDeclarationObject*)> dfsClass; // Declare dfsClass lambda function
    std::function<void(InterfaceDeclarationObject*)> dfsInterface; // Declare dfsInterface lambda function

    dfsClass = [&](ClassDeclarationObject* currentClassObj) {
        for (auto& method : currentClassObj->ast_reference->method_declarations) {
            // TODO: ensure this is not a constructor
            if ( ! method.environment->is_constructor ) {
                allMethods.push_back(method.environment);
            }
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
            // TODO: ensure this is not a constructor
            if ( ! method.environment->is_constructor ) {
                allMethods.push_back(method.environment);
            }
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
        std::unordered_set<MethodDeclaration*> constructor_set;
        std::unordered_set<MethodDeclaration*> method_set;

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
        auto &implements = node.environment->implemented;
        std::unordered_set<InterfaceDeclarationObject*> implementsSet{};
        for(const auto& interface: implements) {
            if ( implementsSet.insert(interface).second == false ) {
                THROW_HierarchyError("An interface must not be mentioned more than once in the same implements clause of a class.");
            }
        }

        std::vector<MethodDeclaration*> constructors;
        std::vector<MethodDeclaration*> methods;

        for ( auto& method : node.method_declarations ) {
            // TODO: constructor boolean not working
            if (method.environment->is_constructor) {
                // constructor
                constructors.push_back(&method);
            } else {
                // method
                methods.push_back(&method);
            }
        }

        // A class must not declare two constructors with the same parameter types (JLS 8.8.2)
        // A class or interface must not declare two methods with the same signature (name and parameter types). (JLS 8.4, 9.4)

        // Check constructor signatures
        if(checkMethodWithSameSignature(constructors)) {
            throw std::runtime_error("Error: Class declares two constructors with the same signature");
        }

        // Check method signatures
        if(checkMethodWithSameSignature(methods)) {
            throw std::runtime_error("Error: Class declares two methods with the same signature");
        }

        // The hierarchy must be acyclic. (JLS 8.1.3, 9.1.2)
        if(checkCyclicHierarchy(classEnv)) {
            throw std::runtime_error("Error: Class has a cyclic hierarchy");
        }

        // A class that contains (declares or inherits) any abstract methods must be abstract. (JLS 8.1.1.1)
        // get all the methods from the class (declared or inherited) and check if any of them are abstract
        if( ! node.hasModifier(Modifier::ABSTRACT) ) {
            std::unordered_set<std::string> implemented_method;
            std::queue<ClassDeclarationObject*> class_queue;
            std::queue<std::vector<InterfaceDeclarationObject*>*> interface_queue;
            class_queue.push(node.environment);

            while ( ! class_queue.empty() ) {
                auto& classObj = class_queue.front();
                class_queue.pop();
                
                // Add implemented methods & error on unimplemented abstract methods
                for ( auto& method : classObj->ast_reference->method_declarations ) {
                    std::string signature = getMethodSignature(method);
                    if ( ! method.hasModifier(Modifier::ABSTRACT) ) {
                        // Non-abstract method
                        implemented_method.insert(signature);
                    } else if ( implemented_method.find(signature) == implemented_method.end() ) {
                        // Abstract method that has not been implemented
                        THROW_HierarchyError("A class that contains (declares or inherits) any abstract methods must be abstract.");
                    }
                }

                if ( classObj->extended ) {
                    class_queue.push(classObj->extended);
                }
                interface_queue.push(&classObj->implemented);
            }

            while ( ! interface_queue.empty() ) {
                auto& interfaces = interface_queue.front();
                interface_queue.pop();

                // Check for unimplemented abstract methods from interfaces
                for ( auto& interface : *interfaces ) {
                    for ( auto& method : interface->ast_reference->method_declarations ) {
                        // All interface methods are abstract
                        std::string signature = getMethodSignature(method);
                        if ( implemented_method.find(signature) == implemented_method.end() ) {
                            // Abstract method that has not been implemented
                            THROW_HierarchyError("A class that contains (declares or inherits) any abstract methods must be abstract.");
                        }
                    }

                    interface_queue.push(&interface->extended);
                }
            }
        }

        /*
        if( ! node.hasModifier(Modifier::ABSTRACT) ) {
            // Non-abstract class
            for(auto& method: allMethods) {
                if( method->ast_reference->hasModifier(Modifier::ABSTRACT) ) {
                    // Abstract method
                    throw std::runtime_error("Error: Class contains abstract method but is not abstract");
                }
            }
        }
        */

        // Perform method checks
        // checkMethods(node.method_declarations, node);

        // Method replacement
        { // Get parent methods
            std::vector<MethodDeclarationObject*> extended_methods;
            std::vector<MethodDeclarationObject*> implemented_methods;

            // Get extended methods
            if ( extendedClass ) {
                extended_methods = getAllMethods(extendedClass);
            } else {
                // No direct superclass
                // => implicitly inherit Object's methods
                // (JLS 8.1.3)

                // Get java.lang.Object
                ClassDeclarationObject* object_class = nullptr;
                try {
                    auto java_package_variant = root_symbol_table->sub_packages->lookupUniqueSymbol("java");
                    auto& java_package = std::get<PackageDeclarationObject>(*java_package_variant);
                    auto lang_package_variant = java_package.sub_packages->lookupUniqueSymbol("lang");
                    auto& lang_package = std::get<PackageDeclarationObject>(*lang_package_variant);
                    auto object_class_variant = lang_package.classes->lookupUniqueSymbol("Object");
                    object_class = &std::get<ClassDeclarationObject>(*object_class_variant);
                } catch (...) {
                    // Error getting java.lang.Object
                    THROW_HierarchyError("java.lang.Object not found");
                }

                // Only apply this check if node is not java.lang.Object
                if ( node.environment != object_class ) {
                    for ( auto& object_method : object_class->ast_reference->method_declarations ) {
                        extended_methods.push_back(object_method.environment);
                    }
                }
            }

            // Get all implemented methods
            for ( auto& superinterface : node.environment->implemented ) {
                for ( auto& implemented_method : getAllMethods(superinterface) ) {
                    implemented_methods.push_back(implemented_method);
                }
            }

            // Check replacement from superclasses
            for ( auto& extended_method : extended_methods ) {
                for ( auto& method : methods ) {
                    if ( *method == *extended_method->ast_reference ) {
                        // Same signature as another method
                        checkMethodReplacement(*method, *extended_method->ast_reference);
                    }
                }
            }

            // Check replacement from superinterfaces
            for ( auto& implemented_method : implemented_methods ) {
                // For class methods
                for ( auto& method : methods ) {
                    if ( *method == *implemented_method->ast_reference ) {
                        // Same signature as class method
                        checkMethodReplacement(*method, *implemented_method->ast_reference);
                    }
                }

                // For extended methods (superclass)
                for ( auto& extended_method : extended_methods ) {
                    if ( *extended_method->ast_reference == *implemented_method->ast_reference) {
                        // Same signature as extended method
                        checkMethodReplacement(*extended_method->ast_reference, *implemented_method->ast_reference);
                    }
                }
            }
        }


        // Check declared/inheritted return types
        auto allMethods = getAllMethods(classEnv);
        for ( auto methodit = allMethods.begin(); methodit != allMethods.end(); methodit++ ) {
            auto& method = (*methodit)->ast_reference;
            for ( auto replacedit = methodit + 1; replacedit != allMethods.end(); replacedit++ ) {
                auto& replaced = (*replacedit)->ast_reference;
                if ( *method == *replaced && *method->type != *replaced->type ) {
                    THROW_HierarchyError("A class or interface must not contain (declare or inherit) two methods with the same signature but different return types.");
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
        std::vector<MethodDeclaration*> methods;
        for ( auto& method : node.method_declarations ) {
            methods.push_back(&method);
        }

        // Add implicit methods for signature checking
        for ( auto& implicit_method : node.implicit_methods ) {
            methods.push_back(&implicit_method);
        }

        // Check that no two methods have the same name and parameter types
        if(checkMethodWithSameSignature(methods)) {
            throw std::runtime_error("Error: Interface declares two methods with the same signature");
        }

        // The hierarchy must be acyclic. (JLS 8.1.3, 9.1.2)
        if(checkCyclicHierarchy(interfaceEnv)) {
            throw std::runtime_error("Error: Interface has a cyclic hierarchy");
        }

        // Check declared/inheritted return types
        auto allMethods = getAllMethods(node.environment);
        for ( auto methodit = allMethods.begin(); methodit != allMethods.end(); methodit++ ) {
            auto& method = (*methodit)->ast_reference;
            for ( auto replacedit = methodit + 1; replacedit != allMethods.end(); replacedit++ ) {
                auto& replaced = (*replacedit)->ast_reference;
                if ( *method == *replaced && *method->type != *replaced->type ) {
                    THROW_HierarchyError("A class or interface must not contain (declare or inherit) two methods with the same signature but different return types.");
                }
            }
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
