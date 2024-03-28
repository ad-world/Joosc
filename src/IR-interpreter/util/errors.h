#pragma once

#include "exceptions/exceptions.h"

struct Trap : public SimulatorError {
    using SimulatorError::SimulatorError;
};

struct InternalCompilerError : public SimulatorError {
    using SimulatorError::SimulatorError;
};
