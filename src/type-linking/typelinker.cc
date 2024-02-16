#include "typelinker.h"
#include <unordered_map>
#include <set>
#include <iostream>

using namespace std;

unordered_map<string, ClassEnvironment> getClassesFromEnvironment(Environment *env) {
    unordered_map<string, ClassEnvironment> result; // Map to store the classes
    // For each child in the environment
    for (auto child: env->getChildren()) {
        // If the child is a class environment
        if (static_cast<ClassEnvironment*>(child) != nullptr) {
            auto &cast = *static_cast<ClassEnvironment*>(child);
            result.emplace(cast.name, cast); // Add the class to the result
        }

        auto classes = getClassesFromEnvironment(child);
        // For each class in the classes
        for (auto &c: classes) {
            result.emplace(c.first, c.second); // This shouldn't be necessary, since all classes should be at the same level
        }
    }

    return result;
}

unordered_map<string, InterfaceEnvironment> getInterfacesFromEnvironment(Environment *env) {
    unordered_map<string, InterfaceEnvironment> result; // Map to store the interfaces
    // For each child in the environment
    for (auto child: env->getChildren()) {
        // If the child is an interface environment
        if (static_cast<InterfaceEnvironment*>(child) != nullptr) {
            auto &cast = *static_cast<InterfaceEnvironment*>(child);
            result.emplace(cast.name, cast); // Add the interface to the result
        }

        // Get the interfaces from the child
        auto interfaces = getInterfacesFromEnvironment(child);
        
        for (auto &interface: interfaces) {
            result.emplace(interface.first, interface.second); // This shouldn't be necessary, since all interfaces should be at the same level
        }
    }

    return result;
}


// This function gets the types in the package
set<string> getPackageTypes(string package_name, vector<CompilationUnit> asts) {
    set<string> types;
    
    // For each ast in the asts
    for (auto &ast: asts) {
        // If the package name matches
        if (ast.package_declaration->getQualifiedName() == package_name) {
            // If there are class declarations, add the class name to the types
            if (ast.class_declarations.size() > 0) {
                string type_name = ast.class_declarations[0].class_name->name;

                // If type with same name exists in the package
                if(types.find(type_name) != types.end()) {
                    cout << "Duplicate type " << type_name << endl;
                    exit(42);
                }

                types.insert(ast.class_declarations[0].class_name->name);
            }

            // If there are interface declarations, add the interface name to the types
            if (ast.interface_declarations.size() > 0) {
                string type_name = ast.interface_declarations[0].interface_name->name;

                // If type with same name exists in the package
                if(types.find(type_name) != types.end()) {
                    cout << "Duplicate type " << type_name << endl;
                    exit(42);
                }

                types.insert(ast.interface_declarations[0].interface_name->name);
            }
        }
    }

    // Return a set of types
    return types;
}


// This function resolves qualified type names "package.Class c;" or "package.Interface i;
AstNodeCommon* resolveQualifiedIdentifier(QualifiedIdentifier *node, Environment *env, string package_name, vector<CompilationUnit> asts) {
    Environment* result = nullptr;
    string type_name = node->getQualifiedName(); // The qualified name of the type

    unordered_map<string, ClassEnvironment> classes = getClassesFromEnvironment(env); // Get classes in env
    unordered_map<string, InterfaceEnvironment> interfaces = getInterfacesFromEnvironment(env); // Get interfaces in env

    if(classes.find(type_name) != classes.end()) {
        result = &classes[type_name]; // If the class is in the environment, set the result to the class
    } else if(interfaces.find(type_name) != interfaces.end()) {
        result = &interfaces[type_name]; // If the interface is in the environment, set the result to the interface
    } 

    // We did not find the type in the environment. Return nullptr. Class doesn't exist, we can't resolve it.
    if(result == nullptr) return nullptr;    

    // Need to get current scope
    unique_ptr<Environment> &current_scope = result->parent; // Get the current scope
    set<string> package_types = getPackageTypes(package_name, asts); // Get the types in the package

    unordered_map<string, ClassEnvironment> current_classes = getClassesFromEnvironment(current_scope.get());
    unordered_map<string, InterfaceEnvironment> current_interfaces = getInterfacesFromEnvironment(current_scope.get());

    string cur_prefix = package_name;

    while (cur_prefix.size() > 0) {
        int last_period = cur_prefix.find_last_of('.');
        
        if (last_period == string::npos) {
            break;
        }

        cur_prefix = cur_prefix.substr(0, last_period); // Create a new prefix

        // Check if the prefix exists in current classes, current interfaces, or package types
        if (current_classes.find(cur_prefix) != current_classes.end() || current_interfaces.find(cur_prefix) != current_interfaces.end() || package_types.find(cur_prefix) != package_types.end()) {
            cout << "Found a prefix class : " << cur_prefix << " of " << type_name << endl;
            exit(42);
        }
    }

    // Return the result AstNodeCommon
    return result->node;
}

AstNodeCommon* checkCurrentPackage(vector<CompilationUnit> asts, string package_name, string type_name) {
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

AstNodeCommon *checkSingleImports(vector<QualifiedIdentifier> single_type_import_declarations, vector<CompilationUnit> asts, string type_name) {
    for (auto &import: single_type_import_declarations) {
        string import_prefix = import.getPackagePrefix(); // get package prefix
        // java.lang.String -> package name = java.lang
            
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

AstNodeCommon *checkTypeOnDemandImports(vector<QualifiedIdentifier> type_import_on_demand_declarations,  vector<CompilationUnit> asts, string type_name, CompilationUnit *current_ast) {
   // Check on demand imports for current idenfitier
    bool found = false;
    AstNodeCommon* result = nullptr;

    // For each on demand import
    for (auto &import: type_import_on_demand_declarations) {
        string import_prefix = import.getPackagePrefix(); // Get the prefix, ie. the qualified name of the package

        for (const CompilationUnit &ast: asts) {
            if(&ast != current_ast) { // We don't want to check the current ast, since we only care about the imported packages / classes
                if(ast.package_declaration->getQualifiedName() == import_prefix) {
                    // If the package name of the AST is the same as the import prefix
                    if (ast.class_declarations.size() > 0 && ast.class_declarations[0].class_name->name == type_name) {
                        // If we have already found the type, we have an ambiguous type
                        if(found) {
                            cout << "Ambiguous type" << endl;
                            exit(42);
                        }

                        // Set the result to the class declaration, and mark the type as found
                        result = ast.class_declarations[0].environment->node;
                        found = true;
                    }

                    if(ast.interface_declarations.size() > 0 && ast.interface_declarations[0].interface_name->name == type_name) {
                        // If we have already found the type, we have an ambiguous type
                        if(found) {
                            cout << "Ambiguous type" << endl;
                            exit(42);
                        }

                        // Set the result to the interface declaration, and mark the type as found
                        result = ast.interface_declarations[0].environment->node;
                        found = true;
                    }
                }
            }
        }
    }

    return result;
}

// This function resolves simple type names "Class c;" or "Interface i;
AstNodeCommon* resolveIdentifier(Identifier *node, Environment *env, string package_name, vector<CompilationUnit> asts, CompilationUnit *current_ast) {
    AstNodeCommon* result = nullptr;
    string type_name = node->name; // string of the type name
    
    unordered_map<string, ClassEnvironment> classes = getClassesFromEnvironment(env); // get classes from environment
    unordered_map<string, InterfaceEnvironment> interfaces = getInterfacesFromEnvironment(env); // get interfaces from environment

    if(classes.find(type_name) != classes.end()) {
        return classes[type_name].node; // if class is in current environment, return it
    } else if(interfaces.find(type_name) != interfaces.end()) {
        return interfaces[type_name].node; // if interface is in current environment, return it
    } 

    vector<QualifiedIdentifier> single_type_import_declarations = current_ast->single_type_import_declaration; // get single imports
    vector<QualifiedIdentifier> type_import_on_demand_declarations = current_ast->type_import_on_demand_declaration; // get on demand imports

    // On demand declarations
    // import java.lang.*

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

TypeLinker::TypeLinker(Environment *env, CompilationUnit *ast_root, vector<CompilationUnit> &asts) : environment{move(env)}, ast_root{move(ast_root)},  asts{asts} {};

// Todo: change this to Type
void TypeLinker::operator()(Type &node) {
    if(holds_alternative<QualifiedIdentifier>(*node.non_array_type)) { // Check that the non_array_type is a QualifiedIdentifier
        QualifiedIdentifier id = *get<unique_ptr<QualifiedIdentifier>>(*node.non_array_type); 
        AstNodeCommon* result;
        if (id.identifiers.size() > 1) {
            result = resolveQualifiedIdentifier(&id, environment, package_name, asts); // Resolve the qualified identifier
        } else {
            Identifier one_id = id.identifiers[0];
            result = resolveIdentifier(&one_id, environment, package_name, asts, ast_root); // Resolve the simple identifier
        }

        if(result == nullptr) {
            throw SemanticError("Type " + id.getQualifiedName() + " not found"); // Type not found
        }

        node.node = result; // Set the result
    }
    
}

