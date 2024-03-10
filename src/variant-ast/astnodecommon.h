#pragma once
// For putting common decorator information e.g. Type
#include "parsing/bison/location.hh"
struct AstNodeCommon {
    virtual ~AstNodeCommon() = default;
    yy::location location;
};
