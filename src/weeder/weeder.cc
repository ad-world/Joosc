#include "weeder.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <variant>

Weeder::Weeder() {
    util = new Utils();
}

void Weeder::checkAsciiRange(const std::string& source) {
    for (char c : source) {
        if (c < 0 || c > 127) {
            addViolation("Character outside 7-bit ASCII range detected: " + c);
            break;
        }
    }
}

void Weeder::addViolation(const std::string& message) {
    violations.push_back(message);
}

void Weeder::printViolations() {
    std::cerr << "Weeder detected the following violations:\n";
    for (const auto& violation : violations) {
        std::cerr << "  - " << violation << '\n';
    }
}

int Weeder::weed(AstNode* root, std::string filename) {
    std::vector<AstNode*> classes = util->getClasses(root);
    std::string file = getFilename(filename);

    // weed program, if we found errors, return 42, else return 0
    checkClassModifiersAndConstructors(classes, file);
    checkLiterals(root);

    if(!violations.empty()) {
        printViolations();
        return 42;
    }

    return 0;
}



void Weeder::checkClassModifiersAndConstructors(std::vector<AstNode*> classes, std::string filename) {
    bool classNameFound = false;

    for (auto c_class : classes) {
        // ------------------- Finding Class Constructor --------------------
        bool constructorFound = false;
        std:string className = util->getClassName(c_class);

        // Check if className == fileName for weeding
        if (className == filename) classNameFound = true;
   
        std::vector<AstNode*> methods = util->getFunctionsFromClass(c_class);

        // Weeding all methods
        checkMethodModifiersAndBody(methods);

        for (auto method: methods) {
            std::string functionName = util->getFunctionName(method);

            if(functionName == className) {
                constructorFound = true;
                break;
            }
        }

        if (!constructorFound) {
            addViolation(className + " does not have a matching constructor.");
        }
        // ------------------- Class cannot be abstract and final ------------
        std::vector<std::string> classModifiers = util->getClassModifiers(c_class);
        std::string abstract_token = "ABSTRACT";
        std::string final_token = "FINAL";
        auto abstractIt = std::find(classModifiers.begin(), classModifiers.end(), abstract_token);
        auto finalIt = std::find(classModifiers.begin(), classModifiers.end(), final_token);

        if ((abstractIt != classModifiers.end()) and (finalIt != classModifiers.end())) {
            addViolation(className + " cannot be both abstract and final.");
        }
    }

    if(!classNameFound) {
        addViolation("No matching class found for " + filename);
    }
}

void Weeder::checkMethodModifiersAndBody(std::vector<AstNode*> methods) {
    for (auto method: methods) {
        // --------------Check method has body if NOT native OR abstract --------
        std::vector<std::string> modifiers = util->getFunctionModifiers(method);
        std::string functionName = util->getFunctionName(method);
        bool nativeOrAbstract = false;

        for (auto modifier: modifiers) {
            if (modifier == "NATIVE" || modifier == "ABSTRACT") nativeOrAbstract = true;

            if (nativeOrAbstract && util->hasFunctionBody(method)) {
                addViolation(functionName + " cannot be abstract/native and have a function body.");
                break;
            }
        }

        // ------------- Check that if not native or abstract, then body is present ------
        if(!nativeOrAbstract && !util->hasFunctionBody(method)) {
            addViolation("Non abstract/native method " + functionName + " must have a function body.");
        }

        // ------------- Check that there is atleast one modifier ------------------------
        if (modifiers.size() == 0) {
            addViolation(functionName + " cannot have 0 access modifiers.");
        }   

        // ------------- Check that modifiers don't include STATIC and FINAL -------------
        std::string final_token = "FINAL";
        std::string static_token = "STATIC";
        
        auto staticIt = std::find(modifiers.begin(), modifiers.end(), static_token);
        auto finalIt = std::find(modifiers.begin(), modifiers.end(), final_token);

        if ((staticIt != modifiers.end()) and (finalIt != modifiers.end())) {
            addViolation(functionName + " cannot be both static and final.");
        }

        // ------------ Check no this() or super() ---------------------------------------
        std::vector<AstNode*> invocations = util->getMethodInvocations(method);

        for (auto invoc: invocations) {
            std::optional<std::variant<std::string, long int>> funcNameOpt = invoc->children[0]->value;

            std::string funcName = "";
            
            if (funcNameOpt) {
                std::variant<std::string, long int> funcNameVar = *funcNameOpt;
                funcName = std::get<std::string>(funcNameVar);
            }

            if(funcName == "super") addViolation("No super() calls allowed inside functions. (" + functionName + ")" );
            if(funcName == "this") addViolation("No this() calls allowed inside functions. (" + functionName + ")");
        }
    }
}

std::string Weeder::getFilename(std::string& filename) {
    // Find the last occurrence of the path separator (/ or \)
    size_t lastSlash = filename.find_last_of("/\\");
    
    // Find the last occurrence of the dot (.) after the last path separator
    size_t lastDot = filename.find_last_of('.');
    
    // Check if the dot is present and comes after the last path separator
    if (lastDot != std::string::npos && (lastSlash == std::string::npos || lastDot > lastSlash)) {
        // Extract the substring between the last path separator and the dot
        return filename.substr(lastSlash + 1, lastDot - lastSlash - 1);
    } else {
        // If no dot or it comes before the last path separator, return the entire file name
        return filename.substr(lastSlash + 1);
    }
}

void Weeder::checkLiterals(AstNode * root) {
    vector<pair<AstNode *, AstNode *>> literals = util->getLiteralPairs(root);
    for ( auto pair : literals ) {
        auto & innerValue = pair.second->value.value();
        switch ( pair.second->type ) {
            case yy::parser::symbol_kind::S_INTEGER:
                // Check range of integer
                if ( pair.first != nullptr && pair.first->type == yy::parser::symbol_kind::S_MINUS ) {
                    if ( -get<long int>(innerValue) < INT32_MIN ) {
                        throw runtime_error("Integer out of range");
                    }
                } else if ( get<long int>(innerValue) > INT32_MAX ) {
                    throw runtime_error("Integer out of range");
                }
                break;
            case yy::parser::symbol_kind::S_STRING_LITERAL:
                // Eventually need to unescape the characters, prob do this in Flex
                break;
            case yy::parser::symbol_kind::S_CHAR_LITERAL:
                // Eventually need to unescape the characters, prob do this in Flex
                break;
        }
    }
}
