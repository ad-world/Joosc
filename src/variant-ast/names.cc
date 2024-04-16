#include "names.h"

#include "exceptions/exceptions.h"

std::string QualifiedIdentifier::getQualifiedName() {
    std::string result = "";
    for (int i = 0; i < identifiers.size(); i++) {
        auto id = identifiers[i];
        if (i == identifiers.size() - 1) {
            result += id.name;
        } else {
            result += id.name + ".";
        }
    }
    return result;
}

std::string QualifiedIdentifier::getFullUnderlyingQualifiedName() {
    if (identifiers.back().field) {
        return identifiers.back().field->full_qualified_name;
    }
    if (identifiers.back().variable) {
        return identifiers.back().variable->identifier;
    }
    if (identifiers.back().parameter) {
        return identifiers.back().parameter->identifier;
    }

    THROW_CompilerError("No underlying qualified name, possible bug");
}

std::string QualifiedIdentifier::getPackagePrefix() {
    std::string result = "";
    for (int i = 0; i < identifiers.size(); i++) {
        result += identifiers[i].name + ".";
    }

    return result;
}

QualifiedIdentifier QualifiedIdentifier::getQualifiedIdentifierWithoutLast() {
    auto identifiers_copy = identifiers;
    identifiers_copy.pop_back();
    return identifiers_copy;
}
