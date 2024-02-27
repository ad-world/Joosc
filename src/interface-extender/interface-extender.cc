#include "interface-extender.h"
#include "exceptions/exceptions.h"

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
            // TODO: create new method
            // TODO: add new method to interface's AST
        }
    }
}

void InterfaceExtender::visit(AstNodeVariant &node) {
    std::visit(*this, node);
}
