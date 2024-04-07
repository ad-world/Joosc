#pragma once

#include "register-allocator.h"

// Register allocator that spills all variables to the stack.
class BrainlessRegisterAllocator : public RegisterAllocator {

  public:
    int32_t allocateRegisters(Tile* function_body) override;
};
