#pragma once

#include <iostream>
#include <vector>

class Weeder {
public:
    // TODO: send root of parse tree to this function
    int weed();

private:
    std::vector<std::string> violations;

    void checkAsciiRange(const std::string& source);
    
    // TODO: send vector of class nodes to this function. It will check modifiers + constructors
    // It will also check that constructors do not include explicit this() or super()
    void checkClassModifiersAndConstructors();

    // TODO: send vector class of methods to this function. This function will check if there is body if it is not native or abstract.
    // It will also check there are no explicit this() or super() calls if there is a function body
    void checkMethodModifiersAndBody();

    // TODO: figure out how this function will work 
    void checkClassNameWithFile();

    // Add a violation to the vector
    void addViolation(const std::string& vio);

    // Print a list of violations, if found
    void printViolations();
};