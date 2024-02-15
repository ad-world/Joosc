#pragma once

#include "variant-ast/astnode.h"
#include "defaultskipvisitor.h"
#include "environment_class.h"

class HierarchyCheckingVisitor : public DefaultSkipVisitor<void> {
    RootEnvironment root_env;
    

    using DefaultSkipVisitor<void>::operator();
    void operator()(ClassDeclaration &node) override {
        // A class must not extend an interface. (JLS 8.1.3)
        auto &extends = node.extends_class;
        // Check that extends EXISTS in environment, and that extends IS NOT an interface
        
        // A class must not extend a final class. (JLS 8.1.1.2, 8.1.3)
        // Check that extends is NOT final

        // A class must not implement a class. (JLS 8.1.4)
        auto &implements = node.implements;
        // Check that implements EXISTS in environment, and that implements IS NOT a class
        // Also check that implements does not have duplicate interfaces

        // A class must not declare two constructors with the same parameter types (JLS 8.8.2)
        // A class or interface must not declare two methods with the same signature (name and parameter types). (JLS 8.4, 9.4)
        auto &methods = node.method_declarations;
        // Check that no two methods have the same name and parameter types



        this->visit_children(node);
    };

    void operator()(InterfaceDeclaration &node) override {
        // An interface must not extend a class. (JLS 9.1.2)
        auto &extends = node.extends_class;
        // Check that extends EXISTS in environment, and that extends IS NOT a class

        // A class must not declare two constructors with the same parameter types (JLS 8.8.2)
        // A class or interface must not declare two methods with the same signature (name and parameter types). (JLS 8.4, 9.4)
        auto &methods = node.method_declarations;
        // Check that no two methods have the same name and parameter types

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
        HierarchyCheckingVisitor(RootEnvironment root_env) : root_env{std::move(root_env)} {};
};