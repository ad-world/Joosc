#include "assembly.h"

#include <unordered_set>

bool Assembly::isRealRegister(std::string reg) {
    std::unordered_set<std::string> set;

    set.emplace(REG8L_ACCUM);
    set.emplace(REG8H_ACCUM);

    set.emplace(REG8L_BASE);
    set.emplace(REG8H_BASE);

    set.emplace(REG8L_COUNTER);
    set.emplace(REG8H_COUNTER);

    set.emplace(REG8L_DATA);
    set.emplace(REG8H_DATA);

    set.emplace(REG8L_STACKPTR);
    set.emplace(REG8L_STACKBASEPTR);

    set.emplace(REG8L_SOURCE);
    set.emplace(REG8L_DEST);

    set.emplace(REG16_ACCUM);
    set.emplace(REG16_BASE);
    set.emplace(REG16_COUNTER);
    set.emplace(REG16_DATA);

    set.emplace(REG16_STACKPTR);
    set.emplace(REG16_STACKBASEPTR);

    set.emplace(REG16_SOURCE);
    set.emplace(REG16_DEST);

    set.emplace(REG32_ACCUM);
    set.emplace(REG32_BASE);
    set.emplace(REG32_COUNTER);
    set.emplace(REG32_DATA);

    set.emplace(REG32_STACKPTR);
    set.emplace(REG32_STACKBASEPTR);

    set.emplace(REG32_SOURCE);
    set.emplace(REG32_DEST);

    return set.count(reg) ? true : false;
}

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
