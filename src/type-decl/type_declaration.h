#pragma once
#include <variant>

typedef std::variant<class ClassDeclarationObject*, class InterfaceDeclarationObject*> TypeDeclaration;
