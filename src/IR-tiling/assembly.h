#pragma once

#include <string>

// Helper methods for creating x86 assembly instructions
class Assembly {
  public:
    static std::string Label(std::string& name) {
        return name + ":";
    }


};
