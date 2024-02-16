#include "astweeder.h"

using namespace std;

// Helper functions
bool isPublicProtected(vector<Modifier> modifiers) {
    auto pubIt = find(modifiers.begin(), modifiers.end(), Modifier::PUBLIC);
    auto protIt = find(modifiers.begin(), modifiers.end(), Modifier::PROTECTED);

    if (pubIt != modifiers.end() && protIt != modifiers.end()) {
        return false;
    }

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


string readFileToString(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        throw runtime_error("Unable to open file: " + filePath);
    }

    string content;
    string line;
    while (getline(file, line)) {
        content += line + "\n";
    }

    return content;
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
    if(holds_alternative<CompilationUnit>(root)) {
        auto &cu = get<CompilationUnit>(root);
        string file = getFileName(fileName);

        // check at most one type per file
        checkOneTypePerFile(cu);
        
        // check program ascii range
        checkAsciiRange(readFileToString(fileName));

        // check programm interfaces
        checkInterfaces(cu.interface_declarations, file);

        // check if program has any classes
        checkClassModifiersAndConstructors(cu.class_declarations, file);

        // check if program has any literals
        checkLiterals(GrabAllVisitor<Literal>().visit(root));


        if(!violations.empty()) {
            printViolations();
            return 42;
        }    
    } 
    
    return 0;
}

void AstWeeder::checkOneTypePerFile(const CompilationUnit &cu) {
    if (cu.class_declarations.size() + cu.interface_declarations.size() > 1) {
        addViolation("Only one type declaration allowed per file.");
    }
}

void AstWeeder::checkInterfaces(vector<InterfaceDeclaration> &interfaces, string &filename) {
    bool interfaceNameFound = false;

    for (const InterfaceDeclaration& inter: interfaces) {
        auto name = inter.interface_name->name;
        if (name == filename) interfaceNameFound = true;

        vector<Modifier> modifiers = inter.modifiers;

        // check if modifiers do not contain public or protected
        if (!isPublicProtected(modifiers)) {
            addViolation("Interface " + name + " must be public or protected.");
        }
    }

    if (!interfaceNameFound && interfaces.size() > 0) {
        addViolation("Interface does not match file name for file " + filename);
    }
}


void AstWeeder::checkClassModifiersAndConstructors(vector<ClassDeclaration> &classes, string &filename) {
    bool classNameFound = false;

    for (ClassDeclaration& cls: classes) {
        bool constructorFound = false;

        // Check if class name matches file name
        auto name = cls.class_name->name;
        if (name == filename) classNameFound = true;
        
        auto &methods = cls.method_declarations;
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
            addViolation("Class " + name + " must be public or protected.");
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
            continue;
        } 

        // Check that method has access modifiers
        if (!isPublicProtected(modifiers)) {
            addViolation("Method " + name + " cannot have 0 access modifiers");
            continue;
        }

        auto staticIt = std::find(modifiers.begin(), modifiers.end(), Modifier::STATIC);
        auto finalIt = std::find(modifiers.begin(), modifiers.end(), Modifier::FINAL);
        auto abstractIt = std::find(modifiers.begin(), modifiers.end(), Modifier::ABSTRACT);

        // Check not STATIC and FINAL
        if ((staticIt != modifiers.end()) && (finalIt != modifiers.end())) {
            addViolation(name + " cannot be both static and final.");
            continue;
        }

        // Check not ABSTRACT and FINAL
        if((finalIt != modifiers.end()) && (abstractIt != modifiers.end())) {
            addViolation(name + " cannot be both abstract and final.");
            continue;
        }

        // Check not ABSTRACT and STATIC
        if((abstractIt != modifiers.end()) && (staticIt != modifiers.end())) {
            addViolation(name + " abstract method cannot be static.");
            continue;
        }

        auto &body = method.body;

        if (body == nullptr) {
            continue;
        }

        // Check that methdod does not call super() or this()
        vector<MethodInvocation*> invocations = GrabAllVisitor<MethodInvocation>().visit(*body);
         
        for (auto &invoc: invocations) {
            if (holds_alternative<QualifiedIdentifier>(*invoc->method_name)) {
                QualifiedIdentifier ident = get<QualifiedIdentifier>(*invoc->method_name);
                
                int q_ident_size = ident.identifiers.size();
                Identifier last_ident = ident.identifiers[q_ident_size - 1];
                string method_name = last_ident.name;

                if(method_name == "super" || method_name == "this") {
                    addViolation("Method " + name + " cannot call super() or this()");
                }
            }    
        }
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
            addViolation("Field " + field.variable_declarator->variable_name->name + " must be public or protected.");
        }
    }
}

void AstWeeder::checkLiterals(vector<Literal*> literals) {
    for (auto &lit: literals) {
        if (holds_alternative<int64_t>(*lit)) {
            auto intLit = get<int64_t>(*lit);
            if (intLit > INT32_MAX) {
                addViolation("Integer literal " + to_string(intLit) + " is too large");
            } else if (intLit < INT32_MIN) {
                addViolation("Integer literal " + to_string(intLit) + " is too small");
            }
        }
    }
}
