#include "weeder.h"

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
    if (!violations.empty()) {
        std::cerr << "Weeder detected the following violations:\n";
        for (const auto& violation : violations) {
            std::cerr << "  - " << violation << '\n';
        }
    }
}

int Weeder::weed() {
    // weed program, if we found errors, return 42, else return 0
    return 0;
}