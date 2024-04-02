#pragma once

#include <string>

// Helper methods for creating assembly instructions
class Assembly {
  public:
    static std::string Label(std::string& name) {
        return name + ":";
    }
    

};
