#include "weeder.h"
#include <algorithm>


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