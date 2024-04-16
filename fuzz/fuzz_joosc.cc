#include "compiler/compiler.h"

#include "src/joosc_proto.pb.h"
#include "src/libfuzzer/libfuzzer_macro.h"
#include "src/proto_to_joos.h"
#include <iostream>
#include <string>

const std::string stdlib = "./tests/stdlib/";
const std::string stdlib_java = stdlib + "java/";

DEFINE_BINARY_PROTO_FUZZER(const joosc_fuzzer::Class& class_msg) {
    Compiler compiler;
    compiler.setRunIR(true);
    auto strfile = joosc_fuzzer::ClassToString(class_msg);
    // std::cout << strfile << std::endl;
    compiler.addStringFile(strfile);

    { // TODO: Don't hard-code stdlib...
        compiler.addInFile(stdlib_java + "io/OutputStream.java");
        compiler.addInFile(stdlib_java + "io/PrintStream.java");
        compiler.addInFile(stdlib_java + "io/Serializable.java");

        compiler.addInFile(stdlib_java + "lang/Boolean.java");
        compiler.addInFile(stdlib_java + "lang/Byte.java");
        compiler.addInFile(stdlib_java + "lang/Character.java");
        compiler.addInFile(stdlib_java + "lang/Class.java");
        compiler.addInFile(stdlib_java + "lang/Cloneable.java");
        compiler.addInFile(stdlib_java + "lang/Integer.java");
        compiler.addInFile(stdlib_java + "lang/Number.java");
        compiler.addInFile(stdlib_java + "lang/Object.java");
        compiler.addInFile(stdlib_java + "lang/Short.java");
        compiler.addInFile(stdlib_java + "lang/String.java");
        compiler.addInFile(stdlib_java + "lang/System.java");

        compiler.addInFile(stdlib_java + "util/Arrays.java");
    }
    compiler.run();
}

// extern "C" int LLVMFuzzerTestOneInput(const uint8_t * data, size_t size) {
//     return size >= 3 &&
//         data[0] == 'F' &&
//         data[1] == 'U' &&
//         data[2] == 'Z' &&
//         data[3] == 'Z';
// }
