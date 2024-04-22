#include "assembly-refactor.h"
#include "assembly-instruction.h"
#include "assembly-common.h"

std::string EffectiveAddress::toString() {
    if (!(scale == 1 || scale == 2 || scale == 4 || scale == 8)) {
        THROW_CompilerError("Invalid parameter for scale");
    }

    std::string result = base_register;

    // index * scale
    if (index_register != EffectiveAddress::EMPTY_REG) {
        result += " + ";
        if (scale != 1) {
            result += "(" + index_register + " * " + std::to_string(scale) + ")";
        } else {
            result += index_register;
        }
    }

    // displacement
    if (displacement != 0) {
        result += (displacement < 0 ? " - " : " + ");
        result += std::to_string(std::abs(displacement));
    }

    return "[" + result + "]";
}
