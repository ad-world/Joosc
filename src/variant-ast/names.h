#pragma once

#include <string>
#include <vector>
#include "astnodecommon.h"
#include "astnode.h"


struct Identifier: public AstNodeCommon {
    std::string name; // Identifier name

    Identifier(std::string& name) : name(std::move(name)) {}
    Identifier(std::string&& name) : name(std::move(name)) {}
};

struct QualifiedIdentifier: public AstNodeCommon {
    LinkedType link;

    std::vector<Identifier> identifiers; // Vector of identifiers for this qualitfed identifier

    QualifiedIdentifier() {}
    QualifiedIdentifier(std::vector<Identifier>& identifiers) : identifiers(std::move(identifiers)) {}
    QualifiedIdentifier(std::vector<Identifier>&& identifiers) : identifiers(std::move(identifiers)) {}
public: 
    std::string getQualifiedName() {
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

    std::string getPackagePrefix() {
        std::string result = "";
        for (int i = 0; i < identifiers.size(); i++) {
            result += identifiers[i].name + ".";
        }

        return result;
    }

    QualifiedIdentifier getQualifiedIdentifierWithoutLast() {
        auto identifiers_copy = identifiers;
        identifiers_copy.pop_back();
        return identifiers_copy;
    }
};
