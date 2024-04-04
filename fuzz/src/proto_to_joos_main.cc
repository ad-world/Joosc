#include <iostream>
#include <fstream>

#include "proto_to_joos.h"

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::fstream in(argv[i]);
        std::string str((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
        std::cout << str.data();
        std::cout << "// " << argv[i] << std::endl;
        std::cout << joosc_fuzzer::ProtoToJoos(
            reinterpret_cast<const uint8_t *>(str.data()),
            str.size()
        );
    }
}
