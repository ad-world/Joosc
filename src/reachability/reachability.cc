#include "reachability.h"
#include "utillities/overload.h"
#include "variant-ast/expressions.h"

bool ReachabilityVisitor::isConstantExpression(Expression &node) {
    bool is_const = true;

    std::visit(util::overload{
        [&](Literal &lit) { /* pass */ },
        [&](CastExpression &castexpr) {
            // TODO: how to check for string
            if ( !castexpr.link.isPrimitive() && !castexpr.link.getIfNonArrayIsClass() ) {
                is_const = false;
            }
        },
        [&](auto &node) { is_const = false; },
    }, node);

    return is_const;
}
