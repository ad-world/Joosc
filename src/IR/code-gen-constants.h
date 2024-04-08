#pragma once

#include <unordered_map>
#include <string>

class MethodDeclarationObject;

// Conventions that need to be the same across different code gen components
class CGConstants {
    static size_t next_method_id;
    static std::unordered_map<MethodDeclarationObject*, std::string> method_labels;
  public:
    // Produce a label for the method, that is guaranteed to not conflict with any other method.
    //
    // Returns the same label for the same method every time.
    static std::string uniqueMethodLabel(MethodDeclarationObject* method);

    // The prefix for abstract argument registers
    // Abstract argument registers are _ARG0, _ARG1, etc.
    const static inline std::string ABSTRACT_ARG_PREFIX = "_ARG";

    // The abstract return register
    // Placing values in this register returns it to the caller.
    const static inline std::string ABSTRACT_RET = "_RET";
};
