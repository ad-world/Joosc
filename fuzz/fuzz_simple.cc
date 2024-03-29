#include "compiler/compiler.h"

#include "joosc_proto.pb.h"
#include "src/libfuzzer/libfuzzer_macro.h"
// #include "proto_to_joos.h"

DEFINE_PROTO_FUZZER(const joosc_fuzzer::Class& class_msg) {
    // Compiler compiler;
    // compiler.addStringFile(joosc_fuzzer::ClassToString(class_msg));
    // compiler.run();
}

// extern "C" int LLVMFuzzerTestOneInput(const uint8_t * data, size_t size) {
//     return size >= 3 &&
//         data[0] == 'F' &&
//         data[1] == 'U' &&
//         data[2] == 'Z' &&
//         data[3] == 'Z';
// }
