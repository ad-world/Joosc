#include "reachability.h"
#include "utillities/util.h"

bool isJavaLangString(LinkedType &link) {
    return link.getIfNonArrayIsClass() == Util::root_package->findClassDeclaration("java.lang.String");
}

bool CfgReachabilityVisitor::isConstantExpression(Expression &node) {
    bool is_const = true;

    std::visit(util::overload{
        [&](Literal &lit) { /* pass */ },
        [&](CastExpression &castexpr) {
            if ( !castexpr.link.isPrimitive() && !isJavaLangString(castexpr.link) ) {
                is_const = false;
            }
        },
        [&](auto &node) { is_const = false; },
    }, node);

    return is_const;
}

void CfgReachabilityVisitor::operator()(CfgStatement *stmt) {
    // TODO: implement
}

void CfgReachabilityVisitor::operator()(CfgExpression *expr) {
    // TODO: implement
}
