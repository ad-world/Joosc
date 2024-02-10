#include "expressions.h"

Assignment::Assignment(
    std::unique_ptr<Expression>& assigned_to,
    std::unique_ptr<Expression>& assigned_from
) :
    assigned_to{std::move(assigned_to)},
    assigned_from{std::move(assigned_from)}
{}