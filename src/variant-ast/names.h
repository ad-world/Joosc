#pragma once

#include <string>
#include <vector>
#include "astnodecommon.h"

struct Identifier: public AstNodeCommon {
    std::string name; // Identifier name

    Identifier(std::string name) : name(std::move(name)) {}
};

struct QualifiedIdentifier: public AstNodeCommon {
    std::vector<Identifier> identifiers; // Vector of identifiers for this qualitfed identifier

    QualifiedIdentifier() {}
    QualifiedIdentifier(std::vector<Identifier> identifiers) : identifiers(std::move(identifiers)) {}
};
