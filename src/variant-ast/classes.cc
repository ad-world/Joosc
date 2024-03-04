#include <algorithm>
#include "classes.h"
#include "statements.h"
#include "types.h"
#include "expressions.h"
#include "names.h"
#include "environment-builder/symboltableentry.h"

ClassDeclaration::ClassDeclaration(
    std::vector<Modifier>& modifiers,
    std::unique_ptr<Identifier>& class_name,
    std::unique_ptr<QualifiedIdentifier>& extends_class,
    std::vector<QualifiedIdentifier>& implements,
    std::vector<FieldDeclaration>& field_declarations,
    std::vector<MethodDeclaration>& method_declarations
) :
    modifiers{std::move(modifiers)},
    class_name{std::move(class_name)},
    extends_class{std::move(extends_class)},
    implements{std::move(implements)},
    field_declarations{std::move(field_declarations)},
    method_declarations{std::move(method_declarations)}
{}

InterfaceDeclaration::InterfaceDeclaration(
    std::vector<Modifier>& modifiers,
    std::unique_ptr<Identifier>& interface_name,
    std::vector<QualifiedIdentifier>& extends_class,
    std::vector<MethodDeclaration>& method_declarations
) :
    modifiers{std::move(modifiers)},
    interface_name{std::move(interface_name)},
    extends_class{std::move(extends_class)},
    method_declarations{std::move(method_declarations)}
{}

FieldDeclaration::FieldDeclaration(
        std::vector<Modifier>& modifiers,
        std::unique_ptr<Type>& type,
        std::unique_ptr<VariableDeclarator>& variable_declarator
) :
    modifiers(std::move(modifiers)),
    type(std::move(type)),
    variable_declarator(std::move(variable_declarator))
{}

MethodDeclaration::MethodDeclaration(
    std::vector<Modifier>& modifiers,
    std::unique_ptr<Type>& type,
    std::unique_ptr<Identifier>& function_name,
    std::vector<FormalParameter>& parameters,
    std::unique_ptr<Block>& body
) :
    modifiers{std::move(modifiers)}, 
    type{std::move(type)},
    function_name{std::move(function_name)},
    parameters{std::move(parameters)},
    body{std::move(body)}
{}

VariableDeclarator::VariableDeclarator(
    std::unique_ptr<Identifier>& variable_name,
    std::unique_ptr<Expression>& expression
) :
    variable_name{std::move(variable_name)}, 
    expression{std::move(expression)}
{}

FormalParameter::FormalParameter(
    std::unique_ptr<Type>& type, 
    std::unique_ptr<Identifier>& parameter_name
) :
    type{std::move(type)}, 
    parameter_name{std::move(parameter_name)} 
{}

// -----------

ClassDeclaration::ClassDeclaration(
    std::vector<Modifier>&& modifiers,
    std::unique_ptr<Identifier>&& class_name,
    std::unique_ptr<QualifiedIdentifier>&& extends_class,
    std::vector<QualifiedIdentifier>&& implements,
    std::vector<FieldDeclaration>&& field_declarations,
    std::vector<MethodDeclaration>&& method_declarations
) :
    modifiers{std::move(modifiers)},
    class_name{std::move(class_name)},
    extends_class{std::move(extends_class)},
    implements{std::move(implements)},
    field_declarations{std::move(field_declarations)},
    method_declarations{std::move(method_declarations)}
{}

InterfaceDeclaration::InterfaceDeclaration(
    std::vector<Modifier>&& modifiers,
    std::unique_ptr<Identifier>&& interface_name,
    std::vector<QualifiedIdentifier>&& extends_class,
    std::vector<MethodDeclaration>&& method_declarations
) :
    modifiers{std::move(modifiers)},
    interface_name{std::move(interface_name)},
    extends_class{std::move(extends_class)},
    method_declarations{std::move(method_declarations)}
{}

FieldDeclaration::FieldDeclaration(
        std::vector<Modifier>&& modifiers,
        std::unique_ptr<Type>&& type,
        std::unique_ptr<VariableDeclarator>&& variable_declarator
) :
    modifiers(std::move(modifiers)),
    type(std::move(type)),
    variable_declarator(std::move(variable_declarator))
{}

MethodDeclaration::MethodDeclaration(
    std::vector<Modifier>&& modifiers,
    std::unique_ptr<Type>&& type,
    std::unique_ptr<Identifier>&& function_name,
    std::vector<FormalParameter>&& parameters,
    std::unique_ptr<Block>&& body
) :
    modifiers{std::move(modifiers)}, 
    type{std::move(type)},
    function_name{std::move(function_name)},
    parameters{std::move(parameters)},
    body{std::move(body)}
{}

VariableDeclarator::VariableDeclarator(
    std::unique_ptr<Identifier>&& variable_name,
    std::unique_ptr<Expression>&& expression
) :
    variable_name{std::move(variable_name)}, 
    expression{std::move(expression)}
{}

FormalParameter::FormalParameter(
    std::unique_ptr<Type>&& type, 
    std::unique_ptr<Identifier>&& parameter_name
) :
    type{std::move(type)}, 
    parameter_name{std::move(parameter_name)} 
{}



/*
* Helpers
*/

bool MethodDeclaration::hasModifier(Modifier mod_to_check) {
    return std::find(modifiers.begin(), modifiers.end(), mod_to_check) != modifiers.end();
}

bool ClassDeclaration::hasModifier(Modifier mod_to_check) {
    return std::find(modifiers.begin(), modifiers.end(), mod_to_check) != modifiers.end();
}

bool InterfaceDeclaration::hasModifier(Modifier mod_to_check) {
    return std::find(modifiers.begin(), modifiers.end(), mod_to_check) != modifiers.end();
}

std::string FormalParameter::toString() const {
    std::string formalParameterType = "";
    if(std::get_if<QualifiedIdentifier>(&(*type->non_array_type))) {
        auto typeDecl = type->link.linked_type;
        if(std::get_if<ClassDeclarationObject*>(&typeDecl)) {
            formalParameterType += std::get<ClassDeclarationObject*>(typeDecl)->identifier;
        } else {
            formalParameterType += std::get<InterfaceDeclarationObject*>(typeDecl)->identifier;
        }
    } else {
        formalParameterType += getPrimitiveName(std::get<PrimitiveType>(*type->non_array_type));
    }
    if(type->is_array) {
        formalParameterType += "[]";
    }
    return formalParameterType;
}

std::string MethodDeclaration::getSignature() const {
    std::string methodSignature = function_name->name + "(";
    bool first = true;
    for(const auto& formalParameter: parameters) {
        if ( first ) { methodSignature += ","; first = false; }
        methodSignature += formalParameter.toString();
    }
    methodSignature += ")";
    return methodSignature;
}
