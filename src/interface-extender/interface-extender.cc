#include "interface-extender.h"
#include "exceptions/exceptions.h"
#include <memory>

void InterfaceExtender::operator()(InterfaceDeclaration &node) {
    if ( ! node.environment->extended.empty() ) {
        // Interface extends something
        return;
    } // Interface has no direct superinterfaces

    // Get java.lang.Object
    ClassDeclarationObject* object_class = nullptr;
    try {
        auto java_package_variant = root_package->sub_packages->lookupUniqueSymbol("java");
        auto& java_package = std::get<PackageDeclarationObject>(*java_package_variant);
        auto lang_package_variant = java_package.sub_packages->lookupUniqueSymbol("lang");
        auto& lang_package = std::get<PackageDeclarationObject>(*lang_package_variant);
        auto object_class_variant = lang_package.classes->lookupUniqueSymbol("Object");
        object_class = &std::get<ClassDeclarationObject>(*object_class_variant);
    } catch (...) {
        // Error getting java.lang.Object
        THROW_HierarchyError("java.lang.Object not found");
    }

    // Loop through object's methods
    for ( auto& method : object_class->ast_reference->method_declarations ) {
        // Create public abstract methods from non-static public methods
        if ( method.hasModifier(Modifier::PUBLIC) && !method.hasModifier(Modifier::STATIC) ) {
            // public && NOT static

            // Copy old method as PUBLIC ABSTRACT
            std::vector<Modifier> new_modifiers = {Modifier::PUBLIC, Modifier::ABSTRACT};
            std::unique_ptr<Type> new_type = std::make_unique<Type>(
                std::make_unique<NonArrayType>(*method.type->non_array_type),
                method.type->is_array
            );
            std::unique_ptr<Identifier> new_function_name = std::make_unique<Identifier>(method.function_name->name);
            std::vector<FormalParameter> new_parameters = { /* POPULATED BELOW */ };
            std::unique_ptr<Block> new_body = nullptr;
            
            // Copy old parameters (populate new array)
            for ( auto& param : method.parameters ) {
                std::unique_ptr<Type> new_param_type = std::make_unique<Type>(
                    std::make_unique<NonArrayType>(*param.type->non_array_type),
                    param.type->is_array
                );
                std::unique_ptr<Identifier> new_param_name = std::make_unique<Identifier>(param.parameter_name->name);
                new_parameters.push_back({new_param_type, new_param_name});
            }

            // Create & add new method to interface's AST
            node.method_declarations.push_back({
                new_modifiers,
                new_type,
                new_function_name,
                new_parameters,
                new_body
            });

            // TODO: how do we link to node.environment->methods?
        }
    }
}

void InterfaceExtender::visit(AstNodeVariant &node) {
    std::visit(*this, node);
}
