#include "typelinker.h"
#include <unordered_map>
#include <set>
#include <iostream>

std::unordered_map<std::string, ClassEnvironment> getClassesFromEnvironment(Environment *env) {
    std::unordered_map<std::string, ClassEnvironment> result;
    for (auto child: env->getChildren()) {
        if (static_cast<ClassEnvironment*>(child) != nullptr) {
            auto &cast = *static_cast<ClassEnvironment*>(child);
            result.emplace(cast.name, cast);
        }

        auto classes = getClassesFromEnvironment(child);
        
        for (auto &c: classes) {
            result.emplace(c.first, c.second);
        }
    }

    return result;
}

std::unordered_map<std::string, InterfaceEnvironment> getInterfacesFromEnvironment(Environment *env) {
    std::unordered_map<std::string, InterfaceEnvironment> result;
    for (auto child: env->getChildren()) {
        if (static_cast<InterfaceEnvironment*>(child) != nullptr) {
            auto &cast = *static_cast<InterfaceEnvironment*>(child);
            result.emplace(cast.name, cast);
        }

        auto interfaces = getInterfacesFromEnvironment(child);
        
        for (auto &interface: interfaces) {
            result.emplace(interface.first, interface.second);
        }
    }

    return result;
}



std::set<std::string> getPackageTypes(std::string package_name, std::vector<CompilationUnit> asts) {
    std::set<std::string> types;

    for (auto &ast: asts) {
        if (ast.package_declaration->getQualifiedName() == package_name) {
            if (ast.class_declarations.size() > 0) {
                types.insert(ast.class_declarations[0].class_name->name);
            }

            if (ast.interface_declarations.size() > 0) {
                types.insert(ast.interface_declarations[0].interface_name->name);
            }
        }
    }

    return types;
}



AstNodeCommon* resolveQualifiedIdentifier(QualifiedIdentifier *node, Environment *env, std::string package_name, std::vector<CompilationUnit> asts) {
    Environment* result = nullptr;
    std::string type_name = node->getQualifiedName();

    std::unordered_map<std::string, ClassEnvironment> classes = getClassesFromEnvironment(env);
    std::unordered_map<std::string, InterfaceEnvironment> interfaces = getInterfacesFromEnvironment(env);

    if(classes.find(type_name) != classes.end()) {
        result = &classes[type_name];
    } else if(interfaces.find(type_name) != interfaces.end()) {
        result = &interfaces[type_name];
    } 

    // We did not find the type in the environment. Return nullprt
    if(result == nullptr) return nullptr;    

    // Need to get current scope
    std::unique_ptr<Environment> &current_scope = result->parent;
    std::set<std::string> package_types = getPackageTypes(package_name, asts);

    std::unordered_map<std::string, ClassEnvironment> current_classes = getClassesFromEnvironment(current_scope.get());
    std::unordered_map<std::string, InterfaceEnvironment> current_interfaces = getInterfacesFromEnvironment(current_scope.get());

    std::string cur_prefix = package_name;

    while (cur_prefix.size() > 0) {
        int last_period = cur_prefix.find_last_of('.');
        
        if (last_period == std::string::npos) {
            break;
        }

        cur_prefix = cur_prefix.substr(0, last_period);

        if (current_classes.find(cur_prefix) != current_classes.end() || current_interfaces.find(cur_prefix) != current_interfaces.end() || package_types.find(cur_prefix) != package_types.end()) {
            std::cout << "Found " << cur_prefix << std::endl;
            exit(42);
        }
    }

    return result->node;
}

AstNodeCommon* checkCurrentPackage(std::vector<CompilationUnit> asts, std::string package_name, std::string type_name) {
    // Checking current package
    for (auto &ast: asts) {
        // If the package name is the same as the current package
        if(ast.package_declaration->getQualifiedName() == package_name) {
            // If the class name is the same as the type name, return that class
            if (ast.class_declarations.size() > 0 && ast.class_declarations[0].class_name->name == type_name) {
                return ast.class_declarations[0].environment->node;
            }

            // If the interface name is the same as the type name, return that interface
            if(ast.interface_declarations.size() > 0 && ast.interface_declarations[0].interface_name->name == type_name) {
                return ast.interface_declarations[0].environment->node;
            }
        }
    }

    return nullptr;
}

AstNodeCommon *checkSingleImports(std::vector<QualifiedIdentifier> single_type_import_declarations, std::vector<CompilationUnit> asts, std::string type_name) {
    for (auto &import: single_type_import_declarations) {
        std::string import_prefix = import.getPackagePrefix(); // get package prefix
            
        for (const CompilationUnit &ast: asts) {
            if(ast.package_declaration->getQualifiedName() == import_prefix) { // if the package name is the same as the import prefix
                if (ast.class_declarations.size() > 0 && ast.class_declarations[0].class_name->name == type_name) {
                    // check that ast class declaration
                    return ast.class_declarations[0].environment->node;
                }

                if(ast.interface_declarations.size() > 0 && ast.interface_declarations[0].interface_name->name == type_name) {
                    // check that ast interface declaration
                    return ast.interface_declarations[0].environment->node;
                }
            }
        }
    }

    return nullptr;
}

AstNodeCommon *checkTypeOnDemandImports(std::vector<QualifiedIdentifier> type_import_on_demand_declarations,  std::vector<CompilationUnit> asts, std::string type_name, CompilationUnit &current_ast) {
   // Check on demand imports for current idenfitier
    bool found = false;
    AstNodeCommon* result = nullptr;

    for (auto &import: type_import_on_demand_declarations) {
        std::string import_prefix = import.getPackagePrefix();

        for (const CompilationUnit &ast: asts) {
            if(&ast != &current_ast) {

                if(ast.package_declaration->getQualifiedName() == import_prefix) {
                    if (ast.class_declarations.size() > 0 && ast.class_declarations[0].class_name->name == type_name) {
                        if(found) {
                            std::cout << "Ambiguous type" << std::endl;
                            exit(42);
                        }

                        result = ast.class_declarations[0].environment->node;
                        found = true;
                    }

                    if(ast.interface_declarations.size() > 0 && ast.interface_declarations[0].interface_name->name == type_name) {
                        if(found) {
                            std::cout << "Ambiguous type" << std::endl;
                            exit(42);
                        }

                        result = ast.interface_declarations[0].environment->node;
                        found = true;
                    }
                }
            }
        }
    }

    return result;
}


AstNodeCommon* resolveIdentifier(Identifier *node, Environment *env, std::string package_name, std::vector<CompilationUnit> asts, CompilationUnit current_ast) {
    AstNodeCommon* result = nullptr;
    std::string type_name = node->name;
    
    std::unordered_map<std::string, ClassEnvironment> classes = getClassesFromEnvironment(env); // get classes from environment
    std::unordered_map<std::string, InterfaceEnvironment> interfaces = getInterfacesFromEnvironment(env); // get interfaces from environment

    if(classes.find(type_name) != classes.end()) {
        return classes[type_name].node; // if class is in current environment, return it
    } else if(interfaces.find(type_name) != interfaces.end()) {
        return interfaces[type_name].node; // if interface is in current environment, return it
    } 

    std::vector<QualifiedIdentifier> single_type_import_declarations = current_ast.single_type_import_declaration; // get single imports
    std::vector<QualifiedIdentifier> type_import_on_demand_declarations = current_ast.type_import_on_demand_declaration; // get on demand imports

    // Check single type imports for current identifier
    result = checkSingleImports(single_type_import_declarations, asts, type_name); // check single imports
    if (result != nullptr) return result;

    result = checkCurrentPackage(asts, package_name, type_name); // check current package
    if (result != nullptr) return result; 

    result = checkTypeOnDemandImports(type_import_on_demand_declarations, asts, type_name, current_ast); // check on demand imports
  
    return result;
}



void TypeLinker::operator()(CompilationUnit &node) {
    package_name = node.package_declaration->getQualifiedName();
    single_type_import_declarations = node.single_type_import_declaration;
    type_import_on_demand_declarations = node.type_import_on_demand_declaration;
    this->visit_children(node);
}

TypeLinker::TypeLinker(Environment *env, CompilationUnit *ast_root, std::vector<CompilationUnit> &asts) : environment{move(env)}, ast_root{move(ast_root)},  asts{asts} {};

void TypeLinker::operator()(QualifiedIdentifier &node) {
    std::string name = node.getQualifiedName();
    if (node.identifiers.size() > 1) {

        // resolve qualified identifier
    } else {
        Identifier &id = node.identifiers[0];
        AstNodeCommon* result = resolveIdentifier(&id, environment, package_name, asts, *ast_root);
    }

}

