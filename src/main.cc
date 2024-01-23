#include <iostream>
#include "factorial/factorial.h"
#include "parsing/parseuser.h"

int main(int argc, char *argv[]) {
    std::cout << factorial(3) << std::endl;
    std::cout << "Hello world" << std::endl;

    return parsemain(argc, argv);
}
