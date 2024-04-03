#include "assembly.h"

size_t Assembly::abstract_reg_count = 0;

std::string Assembly::MakeAddress(
    std::string base_register, 
    std::string index_register,
    int scale,
    int displacement
) {
    int abs_scale = std::abs(scale);
    if (!(abs_scale == 1 || abs_scale == 2 || abs_scale == 4 || abs_scale == 8)) {
        THROW_CompilerError("Invalid parameter for scale");
    }

    std::string result = base_register;

    // index * scale
    if (index_register != "") {
        result += (scale < 0 ? " - " : " + ");
        if (abs_scale != 1) {
            result += "(" + index_register + " * " + std::to_string(abs_scale) + ")";
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