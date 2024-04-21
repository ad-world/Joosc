#pragma once

#include "register-allocator.h"

// Register allocator that does nothing (doesn't produce working programs, but useful for debugging).
class NoopRegisterAllocator : public RegisterAllocator {

  public:
    int32_t allocateRegisters(Tile* function_body) override { return 0; }
};
