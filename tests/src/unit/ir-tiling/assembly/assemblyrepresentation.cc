#include <gtest/gtest.h>

#include "IR-tiling/assembly/assembly-refactor.h"
#include "IR-tiling/assembly/assembly-instruction.h"

// Test that the assembly representation works

TEST(Mov, StringifiesCorrectly) {
    Operand a = "a";
    AssemblyInstruction move_basic = Mov("a", "b");

    EXPECT_TRUE(move_basic.toString() == "mov a b") << "Basic move did not stringify correctly";

    AssemblyInstruction move_effective_address1 = Mov("a", EffectiveAddress("eax", 2));

    EXPECT_TRUE(move_effective_address1.toString() == "mov a [eax + 2]") << "Advanced move 1 did not stringify correctly";
}