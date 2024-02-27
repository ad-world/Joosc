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
        if ( method.type && method.hasModifier(Modifier::PUBLIC) && !method.hasModifier(Modifier::STATIC) ) {
            // non-constructor && public && NOT static

            { // new scope to prevent errors using moved objects
                std::string old_name = method.function_name->name;
                auto& old_non_array = *method.type->non_array_type;

                // Copy old method as PUBLIC ABSTRACT
                std::vector<Modifier> new_modifiers = {Modifier::PUBLIC, Modifier::ABSTRACT};
                std::unique_ptr<Type> new_type = std::holds_alternative<PrimitiveType>(*method.type->non_array_type) ? (
                    // Primitive type
                    std::make_unique<Type>(
                        std::make_unique<NonArrayType>(*method.type->non_array_type),
                        method.type->is_array
                    )
                ): (
                    // Qualified identifier
                    std::make_unique<Type>(
                        std::make_unique<NonArrayType>(
                            QualifiedIdentifier(std::vector<Identifier>(
                                std::get<QualifiedIdentifier>(*method.type->non_array_type).identifiers
                            ))
                        ),
                        method.type->is_array
                    )
                );
                std::unique_ptr<Identifier> new_function_name = std::make_unique<Identifier>(old_name);
                std::vector<FormalParameter> new_parameters = { /* POPULATED BELOW */ };
                std::unique_ptr<Block> new_body = nullptr;

                // Copy old parameters (populate new array)
                for ( auto& param : method.parameters ) {
                    // Create param type
                    std::unique_ptr<Type> new_param_type = std::holds_alternative<PrimitiveType>(*param.type->non_array_type) ? (
                        // Primitive type
                        std::make_unique<Type>(
                            std::make_unique<NonArrayType>(*param.type->non_array_type),
                            param.type->is_array
                        )
                    ): (
                        // Qualified identifier
                        std::make_unique<Type>(
                            std::make_unique<NonArrayType>(
                                QualifiedIdentifier(std::vector<Identifier>(
                                    std::get<QualifiedIdentifier>(*param.type->non_array_type).identifiers
                                ))
                            ),
                            param.type->is_array
                        )
                    );

                    // Create param name
                    std::string old_param_name = param.parameter_name->name;
                    std::unique_ptr<Identifier> new_param_name = std::make_unique<Identifier>(old_param_name);

                    new_parameters.push_back({new_param_type, new_param_name});
                }

                // Create & add new method to interface's AST
                node.implicit_methods.push_back({
                    new_modifiers,
                    new_type,
                    new_function_name,
                    new_parameters,
                    new_body
                });
            } // end of scope

            // Add method to interface
            auto& new_method = node.implicit_methods.back();
            auto new_method_env = node.environment->methods->addSymbol<MethodDeclarationObject>(new_method.function_name->name);

            // Link method
            // TODO: Check if needed
            // new_method.environment = new_method_env;
            // new_method_env->ast_reference = &new_method;

            for ( auto& new_param : new_method.parameters ) {
                // Add parameter to method
                std::string new_param_name = new_param.parameter_name->name;
                auto new_param_env = new_method_env->parameters->addSymbol<FormalParameterDeclarationObject>(new_param_name);

                // Link parameter
                new_param.environment = new_param_env;
                new_param_env->ast_reference = &new_param;
            }
        }
    }
}

void InterfaceExtender::visit(AstNodeVariant &node) {
    std::visit(*this, node);
}
