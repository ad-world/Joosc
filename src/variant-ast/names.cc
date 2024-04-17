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
    if (refersToArrayLength()) {
        return getQualifiedIdentifierWithoutLast().getFullUnderlyingQualifiedName() + ".length";
    }
    if (auto field = getIfRefersToField()) {
        return field->full_qualified_name;
    }
    if (auto variable = getIfRefersToLocalVariable()) {
        return variable->identifier;
    }
    if (auto parameter = getIfRefersToParameter()) {
        return parameter->identifier;
    }

    THROW_CompilerError(
        "No underlying qualified name in '" + getQualifiedName() + "' - likely bug\n"
        "Classification is " + classificationToString(getClassification())
    );
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
