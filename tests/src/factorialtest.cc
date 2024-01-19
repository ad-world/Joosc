#include "../../src/factorial/factorial.h"
#include <gtest/gtest.h>

// Tests factorial of 0.
TEST(factorialTest, FactZero) {
  EXPECT_EQ(factorial(0), 1);
}

// Tests factorial of positive numbers.
TEST(factorialTest, FactPositive) {
  EXPECT_EQ(factorial(1), 1);
  EXPECT_EQ(factorial(2), 2);
  EXPECT_EQ(factorial(3), 6);
  EXPECT_EQ(factorial(8), 40320);
}