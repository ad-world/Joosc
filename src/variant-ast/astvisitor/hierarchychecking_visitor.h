#pragma once

#include "variant-ast/astnode.h"
#include "defaultskipvisitor.h"
#include "environment_class.h"

class HierarchyCheckingVisitor : public DefaultSkipVisitor<void> {
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
};