#include <cstdint>
#include <cstddef>
#include <string>

namespace joosc_fuzzer {
class Class;

std::string ClassToString(const Class &input);
std::string ProtoToJoos(const uint8_t *data, size_t size);
}
