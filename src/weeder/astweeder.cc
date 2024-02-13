#include "astweeder.h"

using namespace std;

// Helper functions
bool isPublicProtected(vector<Modifier> modifiers) {
    auto pubIt = find(modifiers.begin(), modifiers.end(), Modifier::PUBLIC);
    auto protIt = find(modifiers.begin(), modifiers.end(), Modifier::PROTECTED);

    return (pubIt != modifiers.end()) || (protIt != modifiers.end());
}

string getFileName(string& filename) {
    size_t lastSlash = filename.find_last_of("/\\");
    size_t lastDot = filename.find_last_of('.');
    
    if (lastDot != std::string::npos && (lastSlash == std::string::npos || lastDot > lastSlash)) {
        return filename.substr(lastSlash + 1, lastDot - lastSlash - 1);
    } else {
        return filename.substr(lastSlash + 1);
    }
}

void AstWeeder::checkAsciiRange(const string& source) {
    for (char c : source) {
        if (c < 0 || c > 127) {
            addViolation("Character outside 7-bit ASCII range detected: " + c);
            break;
        }
    }
}

void AstWeeder::addViolation(const std::string& vio) {
    violations.push_back(vio);
}

void AstWeeder::printViolations() {
    std::cerr << "Weeder detected the following violations:\n";
    for (const auto& violation : violations) {
        std::cerr << "  - " << violation << '\n';
    }
}

int AstWeeder::weed(AstNodeVariant& root, string fileName) {
    string file = getFileName(fileName);
    // check if program has any interfaces

    if (holds_alternative<CompilationUnit>(root)) {
        auto& cu = get<CompilationUnit>(root);
        // checkAsciiRange();

        checkInterfaces(cu.interface_declarations, file);

        // check if program has any classes
        checkClassModifiersAndConstructors(cu.class_declarations, file);

        // check if program has any literals
        checkLiterals(GrabAllVisitor<Literal>()(root));

        // check if program has any cast expressions
        checkCastExpressionsValid(GrabAllVisitor<CastExpression>()(root));

        if(!violations.empty()) {
            printViolations();
            return 42;
        }    
    }
    
    return 0;
}

void AstWeeder::checkInterfaces(vector<InterfaceDeclaration> &interfaces, string filename) {
    bool interfaceNameFound = false;

    for (const InterfaceDeclaration& inter: interfaces) {
        auto name = inter.interface_name->name;
        if (name == filename) interfaceNameFound = true;

        vector<Modifier> modifiers = inter.modifiers;

        // check if modifiers do not contain public or protected
        if (!isPublicProtected(modifiers)) {
            addViolation("Interface " + name + " does not have access modifiers");
        }
    }

    if (!interfaceNameFound && interfaces.size() > 0) {
        addViolation("Interface does not match file name for file " + filename);
    }
}


void AstWeeder::checkClassModifiersAndConstructors(vector<ClassDeclaration> &classes, string filename) {
    bool classNameFound = false;

    for (ClassDeclaration& cls: classes) {
        bool constructorFound = false;

        // Check if class name matches file name
        auto name = cls.class_name->name;
        if (name == filename) classNameFound = true;
        
        const auto &methods = cls.method_declarations;
        // Check all methods in class
        checkMethodModifiersAndBody(methods);

        const auto &fields = cls.field_declarations;
        // Check all fields in class
        checkClassFields(fields);

        // Try and find class constructor
        for (const MethodDeclaration& method : cls.method_declarations) {
            auto methodName = method.function_name->name;

            if (methodName == name) {
                constructorFound = true;

                auto abstractIt = find(method.modifiers.begin(), method.modifiers.end(), Modifier::ABSTRACT);

                if (abstractIt != method.modifiers.end()) {
                    addViolation("Constructor " + name + " cannot be abstract");
                }

                break;
            }
        }

        if (!constructorFound) {
            addViolation("Constructor " + name + " not found");
        }

        vector<Modifier> modifiers = cls.modifiers;
        // Check that class has an access modifier
        if (!isPublicProtected(modifiers)) {
            addViolation("Class " + name + " does not have access modifiers");
        }

        auto finalIt = find(modifiers.begin(), modifiers.end(), Modifier::FINAL);
        auto abstractIt = find(modifiers.begin(), modifiers.end(), Modifier::ABSTRACT);

        // Check that class is not both final and abstract
        if (finalIt != modifiers.end() && abstractIt != modifiers.end()) {
            addViolation("Class " + name + " cannot be both final and abstract");
        }
    }

    if (!classNameFound && classes.size() > 0) {
        addViolation("No matching class found for " + filename);
    }
}

void AstWeeder::checkMethodModifiersAndBody(const std::vector<MethodDeclaration> &methods) {
    for (const MethodDeclaration& method: methods) {
        auto name = method.function_name->name;
        vector<Modifier> modifiers = method.modifiers;
        bool nativeOrAbstract = false;

        // Check if method is native or abstract
        for (const Modifier& mod: modifiers) {
            if (mod == Modifier::NATIVE || mod == Modifier::ABSTRACT) {
                nativeOrAbstract = true;
                break;
            }
        }

        // Make sure that native or abstract methods do not have a body
        if (nativeOrAbstract && method.body != nullptr) {
            addViolation("Method " + name + " cannot have a body");
        } 

        // Check that method has access modifiers
        if (modifiers.size() == 0) {
            addViolation("Method " + name + " cannot have 0 access modifiers");
        }

        auto staticIt = std::find(modifiers.begin(), modifiers.end(), Modifier::STATIC);
        auto finalIt = std::find(modifiers.begin(), modifiers.end(), Modifier::FINAL);
        auto abstractIt = std::find(modifiers.begin(), modifiers.end(), Modifier::ABSTRACT);

        // Check not STATIC and FINAL
        if ((staticIt != modifiers.end()) && (finalIt != modifiers.end())) {
            addViolation(name + " cannot be both static and final.");
        }

        // Check not ABSTRACT and FINAL
        if((finalIt != modifiers.end()) && (abstractIt != modifiers.end())) {
            addViolation(name + " cannot be both abstract and final.");
        }

        // Check not ABSTRACT and STATIC
        if((abstractIt != modifiers.end()) && (staticIt != modifiers.end())) {
            addViolation(name + " abstract method cannot be static.");
        }

        // TODO: get all method invocations, check that none are super() or this()
    }
}

void AstWeeder::checkClassFields(const vector<FieldDeclaration> &fields) {
    for (const FieldDeclaration& field: fields) {
        vector<Modifier> modifiers = field.modifiers;

        auto finalIt = std::find(modifiers.begin(), modifiers.end(), Modifier::FINAL);

        // Check that field is not final
        if (finalIt != modifiers.end()) {
            addViolation("Field " + field.variable_declarator->variable_name->name + " cannot be final.");
        }

        // Check that field has access modifiers
        if(!isPublicProtected(modifiers)) {
            addViolation("Field " + field.variable_declarator->variable_name->name + " does not have access modifiers");
        }
    }
}

void AstWeeder::checkLiterals(vector<Literal*> literals) {
    // check all literals in new weeder class
    // todo: fill this in
}


void AstWeeder::checkCastExpressionsValid(vector<CastExpression*> exprs) {
    // get all cast expressions
    // todo: fill this function in
    // for (auto &expr: exprs) {
    //     auto primitives = vector<int>{
    //         PrimitiveType::BOOLEAN,
    //         PrimitiveType::BYTE,
    //         PrimitiveType::SHORT,
    //         PrimitiveType::INT,
    //         PrimitiveType::CHAR,
    //     };

    //     Type* cast_type = expr->type;
    //     if (cast_type->is_array) {
    //         addViolation("Cannot cast to an array type");
    //     } else {
    //         auto non_array_type = cast_type->non_array_type;
    //         if (holds_alternative<PrimitiveType>(*non_array_type)) {
    //             auto type = get<PrimitiveType>(*non_array_type);
    //             if (find(primitives.begin(), primitives.end(), type) == primitives.end()) {
    //                 addViolation("Invalid cast to primitive type");
    //             }
    //         }
    //     }
    // }
}

