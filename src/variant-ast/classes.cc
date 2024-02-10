#include "classes.h"
#include "statements.h"
#include "types.h"
#include "expressions.h"
#include "names.h"

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
    std::unique_ptr<QualifiedIdentifier>& extends_class,
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