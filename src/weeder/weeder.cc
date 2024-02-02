#include "weeder.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>


Weeder::Weeder() {
    util = new Utils();
}

void Weeder::checkAsciiRange(const std::string& source) {
    for (char c : source) {
        if (c < 0 || c > 127) {
            addViolation("Character outside 7-bit ASCII range detected.");
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

int Weeder::weed(AstNode* root) {
    std::vector<AstNode*> classes = util->getClasses(root);

    // weed program, if we found errors, return 42, else return 0
    checkClassModifiersAndConstructors(classes);
    checkLiterals(root);

    if(!violations.empty()) {
        printViolations();
        return 42;
    }

    return 0;
}

void Weeder::checkClassModifiersAndConstructors(std::vector<AstNode*> classes) {
    for (auto c_class : classes) {
        // ------------------- Finding Class Constructor --------------------
        bool constructorFound = false;
        std:string className = util->getClassName(c_class);
        std::vector<AstNode*> methods = util->getFunctionsFromClass(c_class);
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
}

void Weeder::checkMethodModifiersAndBody(std::vector<AstNode*> methods) {
    for (auto method: methods) {
        // --------------Check method has body if NOT native OR abstract --------
        std::vector<std::string> modifiers = util->getFunctionModifiers(method);
        std::string functionName = util->getFunctionName(method);
        bool nativeOrAbstract = "false";

        for (auto modifier: modifiers) {
            if (modifier == "NATIVE" || modifier == "ABSTRACT") nativeOrAbstract = true;
            if (nativeOrAbstract && util->hasFunctionBody(method)) {
                addViolation(functionName + " cannot be abstract/native and have a function body.");
                break;
            } else if (!nativeOrAbstract && !util->hasFunctionBody(method)) {
                addViolation("Non native/abstract function " + functionName + " must have a function body.");
                break;
            }
        }
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
        // cout << "(";
        // if ( pair.first != nullptr ) {
        //     cout << util->getParserType(pair.first->type) << ", ";
        // }
        // cout << util->getParserType(pair.second->type) << ")" << endl;
    }
}
