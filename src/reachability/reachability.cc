#include "reachability.h"
#include "utillities/util.h"
#include "variant-ast/expressions.h"
#include <cassert>

bool isJavaLangString(LinkedType &link) {
    return link.getIfNonArrayIsClass() == Util::root_package->findClassDeclaration("java.lang.String");
}

bool CfgReachabilityVisitor::isConstantExpression(Expression &node) {
    bool is_const = true;

    // A compile-time constant expression is an expression denoting a value of
    //  primitive type or a String that is composed using only the following:
    std::visit(util::overload{
        // Literals of primitive type and literals of type String
        [&](Literal &lit) { /* pass */ },

        // Casts to primitive types and casts to type String
        [&](CastExpression &castexpr) {
            if ( !castexpr.link.isPrimitive() && !isJavaLangString(castexpr.link) ) {
                is_const = false;
            }
        },

        // The unary operators +, -, ~, and ! (but not ++ or --)
        [&](PrefixExpression &prefixexpr) { /* pass */ },

        // The multiplicative operators *, /, and %
        // The additive operators + and -
        // The relational operators <, <=, >, and >= (but not instanceof)
        // The equality operators == and !=
        // The bitwise and logical operators &, ^, and |
        // The conditional-and operator && and the conditional-or operator ||
        [&](InfixExpression &infixexpr) { /* pass */ },

        // Simple names that refer to final variables whose initializers are constant expressions
        [&](Identifier &identifier) {
            // TODO: implement
            is_const = false;
        },

        // Qualified names of the form TypeName . Identifier that refer to final variables whose initializers are constant expressions
        [&](QualifiedIdentifier &qid) {
            // TODO: implement
            is_const = false;
        },

        // The shift operators <<, >>, and >>>
        // The ternary conditional operator ? :
        /* Do not exist in Joos*/

        // Error on any other node
        [&](auto &node) { is_const = false; },
    }, node);

    return is_const;
}

bool CfgReachabilityVisitor::evalBoolConstantExpression(Expression &node) {
    assert(isConstantExpression(node));
    // TODO: implement
    return true;
}

void CfgReachabilityVisitor::operator()(CfgStatement *stmt) {
    // TODO: implement
    this->visit_children(stmt);
}

void CfgReachabilityVisitor::operator()(CfgExpression *expr) {
    // TODO: implement
    this->visit_children(expr);
}
