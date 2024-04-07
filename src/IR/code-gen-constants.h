#pragma once

// Constants that need to be the same across different code gen components
namespace CGConstants {
    // The prefix for abstract argument registers
    // Abstract argument registers are _ARG0, _ARG1, etc.
    const std::string ABSTRACT_ARG_PREFIX = "_ARG";

    // The abstract return register
    // Placing values in this register returns it to the caller.
    const std::string ABSTRACT_RET = "_RET";
}
