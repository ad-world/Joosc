#include "hierarchy-checking.h"
#include <map>
#include <stdexcept>
#include <unordered_set>
#include "exceptions/exceptions.h"
#include "variant-ast/astnode.h"
#include "variant-ast/astvisitor/defaultskipvisitor.h"
#include "environment-builder/symboltable.h"
#include "variant-ast/classes.h"
#include "variant-ast/names.h"
#include "variant-ast/primitivetype.h"
#include "variant-ast/types.h"
#include <functional>
#include <variant>
#include <queue>
#include <iostream>

// Return true if they have the same signature
bool checkMethodWithSameSignature(std::vector<MethodDeclaration*>& methods) {
    std::unordered_set<std::string> methodSignatureSet{};
    for(const auto& method: methods) {
        std::string methodSignature = method->getSignature();
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

/* Checks for the following rules
    * 1. A nonstatic method must not replace a static method. (JLS 8.4.6.1)
    * 2. A static method must not replace a nonstatic method. (JLS 8.4.6.2)
    * 3. A method must not replace a method with a different return type. (JLS 8.1.1.1, 8.4, 8.4.2, 8.4.6.3, 8.4.6.4, 9.2, 9.4.1)
    * 4. A protected method must not replace a public method. (JLS 8.4.6.3)
    * 5. A method must not replace a final method. (JLS 8.4.3.3)
    */
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
        THROW_HierarchyError("A protected method must not replace a public method. (" + replaced.function_name->name + ")");
    }

    if ( replaced.hasModifier(Modifier::FINAL) ) {
        THROW_HierarchyError("A method must not replace a final method.");
    }
}

// Get all methods contained (declared or inherited) in a class
std::vector<MethodDeclarationObject*> getAllMethods(std::variant<ClassDeclarationObject*, InterfaceDeclarationObject*> classIntObj) {
    std::vector<MethodDeclarationObject*> allMethods{};
    std::function<void(ClassDeclarationObject*)> dfsClass; // Declare dfsClass lambda function
    std::function<void(InterfaceDeclarationObject*)> dfsInterface; // Declare dfsInterface lambda function

    dfsClass = [&](ClassDeclarationObject* currentClassObj) {
        for (auto& method : currentClassObj->ast_reference->method_declarations) {
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
            if ( ! method.environment->is_constructor ) {
                allMethods.push_back(method.environment);
            }
        }
        for(auto& extendedInterface : currentInterfaceObj->extended) {
            dfsInterface(extendedInterface);
        }
    };

    if ( std::holds_alternative<ClassDeclarationObject*>(classIntObj) ) {
        dfsClass(std::get<ClassDeclarationObject*>(classIntObj));
    } else {
        dfsInterface(std::get<InterfaceDeclarationObject*>(classIntObj));
    }
    return allMethods;
}

// Get all accessible fields contained (declared or inherited) in a class
// Shadowed fields are overriden; the lowest declaration of a field name in the hierachy is used
std::unordered_map<std::string, FieldDeclarationObject*> getAllFields(ClassDeclarationObject* classObj) {
    std::unordered_map<std::string, FieldDeclarationObject*> allFields{};
    std::function<void(ClassDeclarationObject*)> dfsClass; // Declare dfsClass lambda function

    dfsClass = [&](ClassDeclarationObject* currentClassObj) {
        if (currentClassObj->extended != nullptr) {
            dfsClass(currentClassObj->extended);
        }
        for (auto& field : currentClassObj->ast_reference->field_declarations) {
            allFields[field.environment->identifier] = field.environment;
        }
    };

    dfsClass(classObj);
    return allFields;
}


// HierarchyChecking for class declaration
void HierarchyCheckingVisitor::operator()(ClassDeclaration &node) {
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
                std::string signature = method.getSignature();
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
                    std::string signature = method.getSignature();
                    if ( implemented_method.find(signature) == implemented_method.end() ) {
                        // Abstract method that has not been implemented
                        THROW_HierarchyError("A class that contains (declares or inherits) any abstract methods must be abstract.");
                    }
                }

                interface_queue.push(&interface->extended);
            }
        }

        // Find all inherited fields
        node.environment->accessible_fields = getAllFields(node.environment);
    }

    // Method replacement (overriding / hiding)
    // A nonstatic method must not replace a static method. (JLS 8.4.6.1)
    // A static method must not replace a nonstatic method. (JLS 8.4.6.2)
    // A method must not replace a method with a different return type. (JLS 8.1.1.1, 8.4, 8.4.2, 8.4.6.3, 8.4.6.4, 9.2, 9.4.1)
    // A protected method must not replace a public method. (JLS 8.4.6.3)
    // A method must not replace a final method. (JLS 8.4.3.3)

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
        ClassDeclarationObject* object_class = root_symbol_table->getJavaLangObject();

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

    // Methods that are inherited (not overidden, or hidden)
    std::unordered_set<MethodDeclarationObject*> non_replaced_methods;
    for ( auto& extended_method : extended_methods ) {
        non_replaced_methods.insert(extended_method);
    }
    for ( auto& implemented_method : implemented_methods ) {
        non_replaced_methods.insert(implemented_method);
    }


    // Check replacement from superclasses
    for ( auto& extended_method : extended_methods ) {
        for ( auto& method : methods ) {
            if ( *method == *extended_method->ast_reference ) {
                // Same signature as another method
                checkMethodReplacement(*method, *extended_method->ast_reference);
                non_replaced_methods.erase(extended_method);
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
                non_replaced_methods.erase(implemented_method);
            }
        }

        // JLS 8.4.6.4 Inheriting Methods with the Same Signature
        for ( auto& extended_method : extended_methods ) {
            if ( *extended_method->ast_reference == *implemented_method->ast_reference) {
                // Same signature as extended method

                // Check for non-abstract & static overriding abstract
                if ( ! extended_method->ast_reference->hasModifier(Modifier::ABSTRACT) ) {
                    // Non-abstract with abstract
                    if ( extended_method->ast_reference->hasModifier(Modifier::STATIC) ) {
                        THROW_HierarchyError("The method from the superclass is STATIC and non-abstract, causing a compile-time error.");
                    }

                    // Check override
                    checkMethodReplacement(*extended_method->ast_reference, *implemented_method->ast_reference);
                } else {
                    // Both abstract

                    // Check for different return types
                    if ( *extended_method->ast_reference->type != *implemented_method->ast_reference->type ) {
                        THROW_HierarchyError("A method from the superclass is trying to override a method from the superinterface, but they have different return types.");
                    }
                }

                non_replaced_methods.erase(implemented_method);
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

    // Expand symbol_table (all_methods)

    // Add my methods
    for ( auto& my_method : node.method_declarations ) {
        std::string method_name = my_method.environment->identifier;
        node.environment->all_methods.insert({method_name, my_method.environment});
    }
    
    // Add inherited methods (non-replaced)
    for ( auto& inherited_method : non_replaced_methods ) {
        std::string method_name = inherited_method->identifier;
        node.environment->all_methods.insert({method_name, inherited_method});
    }
};



// HierarchyChecking for interface declaration
void HierarchyCheckingVisitor::operator()(InterfaceDeclaration &node) {
    // An interface must not be repeated in an extends clause (JLS 8.1.4)
    auto& interfaceEnv= node.environment;
    auto& extended = interfaceEnv->extended;
    std::unordered_set<InterfaceDeclarationObject*> extendedSet;
    for(const auto& interface: extended) {
        if ( extendedSet.insert(interface).second == false ) {
            THROW_HierarchyError("Error: Interface repeats an interface in its extends clause");
        }
    }

    // A class must not declare two constructors with the same parameter types (JLS 8.8.2)
    // A class or interface must not declare two methods with the same signature (name and parameter types). (JLS 8.4, 9.4)
    std::vector<MethodDeclaration*> methods;
    for ( auto& method : node.method_declarations ) {
        methods.push_back(&method);
    }

    // Add implicit methods for signature checking
    ClassDeclarationObject* object_class = root_symbol_table->getJavaLangObject();
    for ( auto& implicit_method : object_class->ast_reference->method_declarations ) {
        if ( implicit_method.hasModifier(Modifier::PUBLIC) &&
            !implicit_method.hasModifier(Modifier::STATIC)
        ) {
            // Implict method is NON-CONSTRUCTOR, PUBLIC and NON-STATIC
            methods.push_back(&implicit_method);
        }
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

    // Expand symbol_table (all_methods)

    // Get parent methods
    std::vector<MethodDeclarationObject*> parent_methods;
    for ( auto& interface : node.environment->extended ) {
        for ( auto& parent_method : getAllMethods(interface) ) {
            parent_methods.push_back(parent_method);
        }
    }

    // Add my methods
    for ( auto& my_method : node.method_declarations ) {
        std::string method_name = my_method.environment->identifier;
        node.environment->all_methods.insert({method_name, my_method.environment});
    }
    
    // Add implicit Object methods
    for ( auto& implicit_method : object_class->ast_reference->method_declarations ) {
        std::string method_name = implicit_method.environment->identifier;
        node.environment->all_methods.insert({method_name, implicit_method.environment});
    }

    // Add inheritied methods
    for ( auto& parent_method : parent_methods ) {
        std::string method_name = parent_method->identifier;
        node.environment->all_methods.insert({method_name, parent_method});
    }
}

void HierarchyCheckingVisitor::visit(AstNodeVariant &node) {
    std::visit(*this, node);
}

HierarchyCheckingVisitor::HierarchyCheckingVisitor(PackageDeclarationObject &root_symbol_table) :
    root_symbol_table{&root_symbol_table}
{};
