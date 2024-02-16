#include "typelinker.h"
#include <unordered_map>
#include <set>
#include <iostream>
#include <environment-builder/symboltable.h>
#include <variant>

using namespace std;


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
TypeDeclaration resolveQualifiedIdentifier(QualifiedIdentifier *node, PackageDeclarationObject &env, string package_name, vector<CompilationUnit> asts) {
    string type_name = node->getQualifiedName(); // The qualified name of the type

    TypeDeclaration result = static_cast<ClassDeclarationObject*>(nullptr);
    // get classes and interfaces from default package
    auto &classes = env.classes; // Get classes in env
    auto &interfaces = env.interfaces; // Get interfaces in env

    SymbolTableEntry* class_entry = classes.get()->lookupUniqueSymbol(type_name);
    SymbolTableEntry *interface_entry = interfaces.get()->lookupUniqueSymbol(type_name);
    if(class_entry != nullptr) {
        // result = classes.get()->lookupUniqueSymbol(type_name); // If the class is in the environment, set the result to the class
        if(holds_alternative<ClassDeclarationObject>(*class_entry)) {
            result = &std::get<ClassDeclarationObject>(*class_entry);
        }
    } else if(interface_entry != nullptr) {
        if(holds_alternative<InterfaceDeclarationObject>(*interface_entry)) {
            result = &std::get<InterfaceDeclarationObject>(*interface_entry); // If the interface is in the environment, set the result to the interface
        }
    }

    // if(classes.find(type_name) != classes.end()) {
    //     result = &classes[type_name]; // If the class is in the environment, set the result to the class
    // } else if(interfaces.find(type_name) != interfaces.end()) {
    //     result = &interfaces[type_name]; // If the interface is in the environment, set the result to the interface
    // } 

    // We did not find the type in the environment. Return nullptr. Class doesn't exist, we can't resolve it.
    if(holds_alternative<std::nullptr_t>(result)) return static_cast<ClassDeclarationObject*>(nullptr);    

    set<string> package_types = getPackageTypes(package_name, asts); // Get the types in the package

    string cur_prefix = package_name;

    while (cur_prefix.size() > 0) {
        int last_period = cur_prefix.find_last_of('.');
        
        if (last_period == string::npos) {
            break;
        }

        cur_prefix = cur_prefix.substr(0, last_period); // Create a new prefix

        // Check if the prefix exists in current classes, current interfaces, or package types
        if (classes.get()->lookupSymbol(cur_prefix) || interfaces.get()->lookupSymbol(cur_prefix)|| package_types.find(cur_prefix) != package_types.end()) {
            cout << "Found a prefix class : " << cur_prefix << " of " << type_name << endl;
            exit(42);
        }
    }

    // Return the result AstNodeCommon
    return result;
}

PackageDeclarationObject *getPackageObjectFromDeclaration(QualifiedIdentifier* package_decl, PackageDeclarationObject &env) {
    // Starting from the root env, get the package object from a package declaratino
    PackageDeclarationObject *result = &env;
    for (auto &id: package_decl->identifiers) {
        string package_name = id.name;
        // Find the package at the current level of package decl
        if(result->sub_packages.get()->lookupUniqueSymbol(package_name)) {
            // If found, set that as new parent, and then continue
            result = &std::get<PackageDeclarationObject>(*result->sub_packages.get()->lookupUniqueSymbol(package_name));
        } else {
            // Else not found;
            cout << "Package " << package_name << " not found" << endl;
            exit(42);
        }
    }

    return result;
} 

TypeDeclaration checkCurrentPackage(vector<CompilationUnit> asts, string package_name, string type_name) {
    // Checking current package
    for (auto &ast: asts) {
        // If the package name is the same as the current package
        if(ast.package_declaration->getQualifiedName() == package_name) {
            // If the class name is the same as the type name, return that class
            if (ast.class_declarations.size() > 0 && ast.class_declarations[0].class_name->name == type_name) {
                // If we found a class in the package, get the package decl object from the root package, and return the class declaration object
                return ast.class_declarations[0].environment;
            }

            // If the interface name is the same as the type name, return that interface
            if(ast.interface_declarations.size() > 0 && ast.interface_declarations[0].interface_name->name == type_name) {
                // If we found an interface  in the package, get the package decl object from the root package, and return the class declaration object
                return ast.interface_declarations[0].environment;
            }
        }
    }

    return static_cast<ClassDeclarationObject*>(nullptr);
}

TypeDeclaration checkSingleImports(vector<QualifiedIdentifier> single_type_import_declarations, vector<CompilationUnit> asts, string type_name) {
    for (auto &import: single_type_import_declarations) {
        string import_prefix = import.getPackagePrefix(); // get package prefix
        // java.lang.String -> package name = java.lang
            
        for (const CompilationUnit &ast: asts) {
            if(ast.package_declaration->getQualifiedName() == import_prefix) { // if the package name is the same as the import prefix
                if (ast.class_declarations.size() > 0 && ast.class_declarations[0].class_name->name == type_name) {
                    // check that ast class declaration
                    return ast.class_declarations[0].environment;
                }

                if(ast.interface_declarations.size() > 0 && ast.interface_declarations[0].interface_name->name == type_name) {
                    // check that ast interface declaration
                    return ast.interface_declarations[0].environment;
                }
            }
        }
    }

    return static_cast<ClassDeclarationObject*>(nullptr);
}

TypeDeclaration checkTypeOnDemandImports(vector<QualifiedIdentifier> type_import_on_demand_declarations,  vector<CompilationUnit> asts, string type_name, CompilationUnit *current_ast) {
   // Check on demand imports for current idenfitier
    bool found = false;
    TypeDeclaration result = static_cast<ClassDeclarationObject*>(nullptr);

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
                        result = ast.class_declarations[0].environment;
                        found = true;
                    }

                    if(ast.interface_declarations.size() > 0 && ast.interface_declarations[0].interface_name->name == type_name) {
                        // If we have already found the type, we have an ambiguous type
                        if(found) {
                            cout << "Ambiguous type" << endl;
                            exit(42);
                        }

                        // Set the result to the interface declaration, and mark the type as found
                        result = ast.interface_declarations[0].environment;
                        found = true;
                    }
                }
            }
        }
    }

    return result;
}

// This function resolves simple type names "Class c;" or "Interface i;
TypeDeclaration resolveIdentifier(Identifier *node, PackageDeclarationObject &env, string package_name, vector<CompilationUnit> asts, CompilationUnit *current_ast) {
    TypeDeclaration result = static_cast<ClassDeclarationObject*>(nullptr);
    string type_name = node->name; // string of the type name
    
    auto &classes = env.classes; // Get classes in env
    auto &interfaces = env.interfaces; // Get interfaces in env

    SymbolTableEntry* class_entry = classes.get()->lookupUniqueSymbol(type_name);
    SymbolTableEntry *interface_entry = interfaces.get()->lookupUniqueSymbol(type_name);
    if(class_entry != nullptr) {
        // result = classes.get()->lookupUniqueSymbol(type_name); // If the class is in the environment, set the result to the class
        if(holds_alternative<ClassDeclarationObject>(*class_entry)) {
            result = &std::get<ClassDeclarationObject>(*class_entry);
        }
    } else if(interface_entry != nullptr) {
        if(holds_alternative<InterfaceDeclarationObject>(*interface_entry)) {
            result = &std::get<InterfaceDeclarationObject>(*interface_entry); // If the interface is in the environment, set the result to the interface
        }
    }

    vector<QualifiedIdentifier> single_type_import_declarations = current_ast->single_type_import_declaration; // get single imports
    vector<QualifiedIdentifier> type_import_on_demand_declarations = current_ast->type_import_on_demand_declaration; // get on demand imports

    // On demand declarations
    // import java.lang.*

    // Check single type imports for current identifier
    result = checkSingleImports(single_type_import_declarations, asts, type_name); // check single imports
    if (holds_alternative<std::nullptr_t>(result)) return result;

    result = checkCurrentPackage(asts, package_name, type_name); // check current package
    if (holds_alternative<std::nullptr_t>(result)) return result; 

    result = checkTypeOnDemandImports(type_import_on_demand_declarations, asts, type_name, current_ast); // check on demand imports
  
    return result;
}



void TypeLinker::operator()(CompilationUnit &node) {
    package_name = node.package_declaration->getQualifiedName();
    single_type_import_declarations = node.single_type_import_declaration;
    type_import_on_demand_declarations = node.type_import_on_demand_declaration;
    this->visit_children(node);
}

TypeLinker::TypeLinker(PackageDeclarationObject &env, CompilationUnit *ast_root, vector<CompilationUnit> &asts) : root_env{env}, ast_root{move(ast_root)},  asts{asts} {};


void TypeLinker::operator()(Type &node) {
    if(holds_alternative<QualifiedIdentifier>(*node.non_array_type)) { // Check that the non_array_type is a QualifiedIdentifier
        QualifiedIdentifier id = *get<unique_ptr<QualifiedIdentifier>>(*node.non_array_type); 
        TypeDeclaration result = static_cast<ClassDeclarationObject*>(nullptr);
        if (id.identifiers.size() > 1) {
            result = resolveQualifiedIdentifier(&id, root_env, package_name, asts); // Resolve the qualified identifier
        } else {
            Identifier one_id = id.identifiers[0];
            result = resolveIdentifier(&one_id, root_env, package_name, asts, ast_root); // Resolve the simple identifier
        }

        if(holds_alternative<std::nullptr_t>(result)) {
            throw SemanticError("Type " + id.getQualifiedName() + " not found"); // Type not found
        }

        node.node = result; // Set the result
    }
    
}

