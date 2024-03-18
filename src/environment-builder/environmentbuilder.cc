#include <iostream>
#include <string>
#include "environmentbuilder.h"
#include "exceptions/semanticerror.h"
#include "exceptions/compilerdevelopmenterror.h"
#include "exceptions/exceptions.h"
#include "symboltable.h"
#include "symboltableentry.h"

void EnvironmentBuilder::operator()(CompilationUnit &node) {
    if (node.package_declaration) {
        // Not in default package, iterate over package subnames to get to the package for this compilation unit
        for (auto& identifier : node.package_declaration->identifiers) {
            std::string package_subname = identifier.name;

            if (!current_package->is_default_package) {
                // Enclosing package is not the default package; classes must not conflict with package
                if (current_package->classes->lookupSymbol(package_subname) ||
                    current_package->interfaces->lookupSymbol(package_subname)) {
                    THROW_EnvBuilderError("Package name resolves to class/interface name");
                }
            }

            auto possible_package = current_package->sub_packages->lookupUniqueSymbol(package_subname);
            if (possible_package) {
                // Package was previously declared in another AST CompilationUnit and the same package is used
                this->current_package = &(std::get<PackageDeclarationObject>(*possible_package));
            } else {
                // Package statement implictly declares package for the first time
                this->current_package 
                    = current_package->sub_packages->addSymbol<PackageDeclarationObject>(package_subname);
            }
        }
    }
    visit_children(node);
}

void EnvironmentBuilder::operator()(ClassDeclaration &node) {
    std::string class_name = node.class_name->name;

    // Check for any conflicts in package
    if (!current_package->is_default_package) {
        // Class not in default package must not conflict with package name
        if (current_package->sub_packages->lookupSymbol(class_name)) {
            THROW_EnvBuilderError("Class name " + class_name + " resolves to package name");
        }
    }
    if (current_package->classes->lookupSymbol(class_name) || current_package->interfaces->lookupSymbol(class_name)) {
        THROW_EnvBuilderError("Class name " + class_name + " redeclared in package");
    }

    // Class does not conflict in package and can be added
    auto class_env = current_package->classes->addSymbol<ClassDeclarationObject>(class_name);
    class_env->package_contained_in = current_package;
    this->current_type = class_env;

    linkDeclaration(node, *class_env);
    visit_children(node);
}

void EnvironmentBuilder::operator()(InterfaceDeclaration &node) {
    std::string interface_name = node.interface_name->name;

    // Check for any conflicts in package
    if (!current_package->is_default_package) {
        // Interface not in default package must not conflict with package name
        if (current_package->sub_packages->lookupSymbol(interface_name)) {
            THROW_EnvBuilderError("Interface name " + interface_name + " resolves to package name");
        }
    }
    if (current_package->classes->lookupSymbol(interface_name) 
        || current_package->interfaces->lookupSymbol(interface_name)) 
    {
        THROW_EnvBuilderError("Interface name " + interface_name + " redeclared in package");
    }

    // Interface does not conflict in package and can be added
    auto int_env = current_package->interfaces->addSymbol<InterfaceDeclarationObject>(interface_name);
    int_env->package_contained_in = current_package;
    this->current_type = int_env;
    
    linkDeclaration(node, *int_env);
    visit_children(node);
}

void EnvironmentBuilder::operator()(FieldDeclaration &node) {
    std::string field_name = node.variable_declarator->variable_name->name;
    
    // Field declarations only appear in classes
    if (auto current_class_ptr = std::get_if<ClassDeclarationObject*>(&current_type)) {
        // Check for any conflicts in class
        auto current_class = *current_class_ptr;

        if (current_class->fields->lookupSymbol(field_name)) {
            THROW_EnvBuilderError("Field name " + field_name + " redeclared");
        }

        // Field does not conflict in class and can be added
        auto field_env = current_class->fields->addSymbol<FieldDeclarationObject>(field_name);
        field_env->containing_class = current_class;
        linkDeclaration(node, *field_env);
    } else {
        // This should not happen
        throw CompilerDevelopmentError("Found field in interface AST node during environment building");
    }

    visit_children(node);
}

void EnvironmentBuilder::operator()(MethodDeclaration &node) {
    std::string method_name = node.function_name->name;

    // Skip looking for method name conflicts for now, as must be checked in later compiler stage
    // due to overloading meaning we need to check parameter types.

    // Add method to class or interface, whatever the current type may be
    std::visit([&](auto cls_or_int_obj) {
        this->current_method = cls_or_int_obj->methods->template addSymbol<MethodDeclarationObject>(method_name);
        this->current_method->containing_type = cls_or_int_obj;
    }, current_type);

    this->current_method->scope_manager.closeAllScopes();

    linkDeclaration(node, *this->current_method);
    visit_children(node);

    this->current_method->scope_manager.closeAllScopes();
}

void EnvironmentBuilder::operator()(Block &node) {
    node.scope_id = current_method->scope_manager.createNewScope();
    current_method->scope_manager.openScope(node.scope_id);
    visit_children(node);
    current_method->scope_manager.closeScope(node.scope_id);
}

void EnvironmentBuilder::operator()(FormalParameter &node) {
    std::string parameter_name = node.parameter_name->name;

    // Local variables and parameters must not conflict with each other
    if (current_method->parameters->lookupSymbol(parameter_name) || 
            current_method->scope_manager.lookupVariable(parameter_name)) {
                THROW_EnvBuilderError("Parameter name " + parameter_name + " redeclared");
            }
    
    // Add parameter to method
    auto param_env =
        this->current_method->parameters->addSymbol<FormalParameterDeclarationObject>(parameter_name);
    this->current_method->parameter_list.push_back(param_env);

    linkDeclaration(node, *param_env);
    visit_children(node);
}

void EnvironmentBuilder::operator()(LocalVariableDeclaration &node) {
    std::string local_variable_name = node.variable_declarator->variable_name->name;

    // Local variables and parameters must not conflict with each other
    if (current_method->parameters->lookupSymbol(local_variable_name) || 
            current_method->scope_manager.lookupVariable(local_variable_name)) {
                THROW_EnvBuilderError("Local variable name " + local_variable_name + " redeclared");
            }
    
    // Add parameter to method
    auto var_env 
        = this->current_method->scope_manager.addVariable(local_variable_name);

    linkDeclaration(node, *var_env);
    visit_children(node);
}

EnvironmentBuilder::EnvironmentBuilder(PackageDeclarationObject &default_package) :
    current_package{&default_package}, 
    current_type{static_cast<ClassDeclarationObject*>(nullptr)},
    current_method{nullptr} 
{}

void EnvironmentBuilder::visit(AstNodeVariant &node) {
    std::visit(*this, node);
}
