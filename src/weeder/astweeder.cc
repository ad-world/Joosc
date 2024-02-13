#include "astweeder.h"
#include "astnode.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <variant>

using namespace std;

AstWeeder::checkAsciiRange(const string& source) {
    for (char c : source) {
        if (c < 0 || c > 127) {
            addViolation("Character outside 7-bit ASCII range detected: " + c);
            break;
        }
    }
}

void AstWeeder::addViolation(const std::string& message) {
    violations.push_back(message);
}

void AstWeeder::printViolations() {
    std::cerr << "Weeder detected the following violations:\n";
    for (const auto& violation : violations) {
        std::cerr << "  - " << violation << '\n';
    }
}

void AstWeeder::checkInterfaces(vector<InterfaceDeclaration*> interfaces, string filename) {
    bool interfaceNameFound = false;

    for (auto inter: interfaces) {
        auto name = inter->interface_name->name;
        if (name == filename) interfaceNameFound = true;

        vector<Modifier> = inter->modifiers;
    }
}