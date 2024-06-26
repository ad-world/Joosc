#include "reachability.h"
#include "exceptions/exceptions.h"
#include "utillities/overload.h"
#include "utillities/util.h"
#include "variant-ast/expressions.h"
#include "variant-ast/primitivetype.h"
#include <cassert>
#include <string>
#include <variant>

std::unordered_set<Statement*> CfgReachabilityVisitor::reached;

bool isJavaLangString(LinkedType &link) {
    return link.getIfNonArrayIsClass() == Util::root_package->findClassDeclaration("java.lang.String");
}

bool CfgReachabilityVisitor::isConstantExpression(Expression &node) {
    // A compile-time constant expression is an expression denoting a value of
    //  primitive type or a String that is composed using only the following:
    return std::visit(util::overload{
        // Literals of primitive type and literals of type String
        [&](Literal &lit) -> bool { return true; },

        // Casts to primitive types and casts to type String
        [&](CastExpression &castexpr) -> bool {
            if ( !castexpr.link.isPrimitive() && !isJavaLangString(castexpr.link) ) {
                return false;
            }
            if ( castexpr.expression ) {
                return isConstantExpression(*castexpr.expression);
            }
            return false;
        },

        // The unary operators +, -, ~, and ! (but not ++ or --)
        [&](PrefixExpression &prefixexpr) -> bool {
            if ( prefixexpr.expression ) {
                return isConstantExpression(*prefixexpr.expression);
            }
            return false;
        },

        // The multiplicative operators *, /, and %
        // The additive operators + and -
        // The relational operators <, <=, >, and >= (but not instanceof)
        // The equality operators == and !=
        // The bitwise and logical operators &, ^, and |
        // The conditional-and operator && and the conditional-or operator ||
        [&](InfixExpression &infixexpr) -> bool {
            if ( infixexpr.expression1 && infixexpr.expression2 ) {
                if (isConstantExpression(*infixexpr.expression1) && isConstantExpression(*infixexpr.expression2)) {
                    if (infixexpr.op == InfixOperator::DIVIDE ||
                        infixexpr.op == InfixOperator::MODULO
                    ) {
                        // Check for division by 0
                        auto right_lit = evalConstantExpression(*infixexpr.expression2);
                        return std::visit(util::overload{
                            [&](int64_t &right) {
                                // Ensure not dividing by 0
                                return right != 0;
                            },
                            [&](auto &lit) { return true; }
                        }, right_lit);
                    }

                    // Not division or modulo
                    return true;
                }
            }
            return false;
        },

        // Catch parenthesized expressions
        [&](ParenthesizedExpression &parenexpr) -> bool {
            if ( parenexpr.expression ) {
                return isConstantExpression(*parenexpr.expression);
            }
            return false;
        },

        // Simple names that refer to final variables whose initializers are constant expressions
        // Qualified names of the form TypeName . Identifier that refer to final variables whose initializers are constant expressions
        // The shift operators <<, >>, and >>>
        // The ternary conditional operator ? :
        /* Do not exist in Joos*/

        // Error on any other node
        [&](auto &node) -> bool {
            return false;
        },
    }, node);
}

Literal CfgReachabilityVisitor::evalConstantExpression(Expression &node) {
    assert(isConstantExpression(node));

    return std::visit(util::overload{
        [&](Literal &literal) -> Literal {
            return literal;
        },

        [&](CastExpression &castexpr) -> Literal {
            // NARROW CONVERSIONS UNSUPPORTED IN JOOS !!!!!!

            // Evaluate expression literal
            assert(castexpr.expression.get());
            Literal literal = evalConstantExpression(*castexpr.expression);

            // Debugging string
            std::string location = Util::locationToString(castexpr.location);

            // Literal pointers
            auto literal_int64 = std::get_if<int64_t>(&literal);
            auto literal_char = std::get_if<char>(&literal);
            auto literal_bool = std::get_if<bool>(&literal);
            auto literal_nullptr = std::get_if<std::nullptr_t>(&literal);
            auto literal_str = std::get_if<std::string>(&literal);

            if ( auto type = std::get_if<PrimitiveType>(&castexpr.link.linked_type) ) {
                /*  - Implicit integer constant casts are disallowed in joosc
                *   - Narrowing is disallowed in joosc (char only identity conversion)
                *   - There is no permitted conversion to the type boolean other
                *       than the identity conversion. (5.1.7)
                *   - There is no permitted conversion to the null type other than
                *       the identity conversion. (5.1.7)
                */


                switch (*type) {
                    case PrimitiveType::BYTE:   // fallthru
                    case PrimitiveType::SHORT:  // fallthru
                    case PrimitiveType::INT:
                        // TODO: normally should be an error, but stdlib has these
                        // THROW_CompilerError("Implicit integer constant casts are disallowed in joosc: " + location);
                        if ( literal_int64 ) {
                            return *literal_int64;
                        } else if ( literal_char ) {
                            return (int64_t) *literal_char;
                        } else {
                            THROW_CompilerError("Error casting expression to signed int: " + location);
                        }
                    case PrimitiveType::CHAR:
                        if ( literal_char ) {
                            return *literal_char;
                        } else {
                            THROW_CompilerError("Error casting expression to char: " + location);
                        }
                    case PrimitiveType::BOOLEAN:
                        if ( literal_bool ) {
                            return *literal_bool;
                        } else {
                            THROW_CompilerError("Error casting expression to bool: " + location);
                        }
                    case PrimitiveType::NULL_T:
                        if ( literal_nullptr ) {
                            return (std::nullptr_t) nullptr;
                        } else {
                            THROW_CompilerError("Error casting expression to null: " + location);
                        }
                    case PrimitiveType::VOID:
                        THROW_CompilerError("Error cannot cast to void: " + location);
                    default:
                        THROW_CompilerError("Unhandled primitive type: " + location);
                }
            } else {
                // There is a string conversion to type String from every
                //  other type, including the null type. (5.1.6)

                // Cast to a string
                if ( literal_str ) {
                    return *literal_str;
                } else {
                    // TODO: implement for all other types
                    THROW_CompilerError("TODO IMPLEMENT: Cannot yet cast to string type " + location);
                }
            }
        },

        [&](PrefixExpression &prefixexpr) -> Literal {
            // Evaluate expression literal
            assert(prefixexpr.expression);
            Literal literal = evalConstantExpression(*prefixexpr.expression);

            // Debugging string
            std::string location = Util::locationToString(prefixexpr.location);

            if ( auto type = std::get_if<PrimitiveType>(&prefixexpr.link.linked_type) ) {
                switch ( prefixexpr.op ) {
                    case PrefixOperator::MINUS: {
                        if ( auto literal_int64 = std::get_if<int64_t>(&literal) ) {
                            switch (*type) {
                                case PrimitiveType::BYTE:
                                    return -((joos_byte_t) *literal_int64);
                                case PrimitiveType::SHORT:
                                    return -((joos_short_t) *literal_int64);
                                case PrimitiveType::INT:
                                    return -((joos_int_t) *literal_int64);
                                default: break;
                            }
                        }
                        THROW_CompilerError("Error applying minus operator: " + location);
                    }

                    case PrefixOperator::NEGATE: {
                        if ( auto literal_bool = std::get_if<bool>(&literal) ) {
                            switch (*type) {
                                case PrimitiveType::BOOLEAN:
                                    return !*literal_bool;
                                default: break;
                            }
                        }
                        THROW_CompilerError("Error applying negation operator: " + location);
                    }
                }
            }

            THROW_CompilerError("Error handling prefix expression: " + location);
        },

        [&](InfixExpression &infixexpr) -> Literal {
            // Evaluate expression literal
            assert(infixexpr.expression1);
            assert(infixexpr.expression2);
            Literal left = evalConstantExpression(*infixexpr.expression1);
            Literal right = evalConstantExpression(*infixexpr.expression2);

            // Debugging string
            std::string location = Util::locationToString(infixexpr.location);

            // Literal values
            auto left_bool = std::get_if<bool>(&left);
            auto right_bool = std::get_if<bool>(&right);
            auto left_int64 = std::get_if<int64_t>(&left);
            auto right_int64 = std::get_if<int64_t>(&right);
            auto left_char = std::get_if<char>(&left);
            auto right_char = std::get_if<char>(&right);
            auto left_nullptr = std::get_if<std::nullptr_t>(&left);
            auto right_nullptr = std::get_if<std::nullptr_t>(&left);
            auto left_str = std::get_if<std::string>(&left);
            auto right_str = std::get_if<std::string>(&right);

            switch ( infixexpr.op ) {
                case InfixOperator::EAGER_OR:
                case InfixOperator::BOOLEAN_OR:
                    if ( left_bool && right_bool ) {
                        return *left_bool || *right_bool;
                    } else { break; }
                case InfixOperator::BOOLEAN_AND:
                case InfixOperator::EAGER_AND:
                    if ( left_bool && right_bool ) {
                        return *left_bool && *right_bool;
                    } else { break; }
                case InfixOperator::BOOLEAN_EQUAL:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 == *right_int64;
                    } else if ( left_char && right_char ) {
                        return *left_char == *right_char;
                    } else if ( left_str && right_str ) {
                        return *left_str == *right_str;
                    } else if ( ( left_str && right_nullptr ) || ( left_nullptr && right_str ) ) {
                        return false;
                    } else if ( left_nullptr && right_nullptr ) {
                        return true;
                    } else if ( left_bool && right_bool ) {
                        return *left_bool == *right_bool;
                    } else { break; }
                case InfixOperator::BOOLEAN_NOT_EQUAL:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 != *right_int64;
                    } else if ( left_char && right_char ) {
                        return *left_char != *right_char;
                    } else if ( left_str && right_str ) {
                        return *left_str != *right_str;
                    } else if ( ( left_str && right_nullptr ) || ( left_nullptr && right_str ) ) {
                        return true;
                    } else if ( left_nullptr && right_nullptr ) {
                        return false;
                    } else if ( left_bool && right_bool ) {
                        return *left_bool != *right_bool;
                    } else { break; }
                case InfixOperator::PLUS:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 + *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char + *right_char);
                    } else if ( left_str && right_str ) {
                        return *left_str + *right_str;
                    } else if ( left_str || right_str ) {
                        THROW_CompilerError("TODO: Handle implicit string concatentation: " + location);
                    } else { break; }

                // Numeric operators
                case InfixOperator::MINUS:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 - *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char - *right_char);
                    } else { break; }
                case InfixOperator::DIVIDE:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 / *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char / *right_char);
                    } else { break; }
                case InfixOperator::MULTIPLY:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 * *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char * *right_char);
                    } else { break; }
                case InfixOperator::MODULO:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 % *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char % *right_char);
                    } else { break; }
                case InfixOperator::LESS_THAN:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 < *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char < *right_char);
                    } else { break; }
                case InfixOperator::GREATER_THAN:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 > *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char > *right_char);
                    } else { break; }
                case InfixOperator::LESS_THAN_EQUAL:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 <= *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char <= *right_char);
                    } else { break; }
                case InfixOperator::GREATER_THAN_EQUAL:
                    if ( left_int64 && right_int64 ) {
                        return *left_int64 >= *right_int64;
                    } else if ( left_char && right_char ) {
                        return (int64_t) (*left_char >= *right_char);
                    } else { break; }
                default: break;
            }

            THROW_CompilerError("Error handling InfixExpression: " + location);
        },

        [&](ParenthesizedExpression &parenexpr) -> Literal {
            assert(parenexpr.expression);
            return evalConstantExpression(*parenexpr.expression);
        },

        [&](auto &node) -> Literal {
            // Catch-all non-const expressions
            THROW_CompilerError("Evaluation function used on non-constant expression");
        }
    }, node);
}

void CfgReachabilityVisitor::operator()(CfgStatement *stmt) {
    auto current_method_return = current_method->return_type.getIfNonArrayIsPrimitive();

    reached.insert(stmt->statement);
    if ( stmt->is_return ) {
        return;
    } else {
        if ((current_method_return == nullptr || *current_method_return != PrimitiveType::VOID) && !current_method->is_constructor) {
            // We are in a function with a non-void return type, and we are in a statement that is NOT a return statement
            std::string location_str;
            if(stmt->statement) {
                location_str = Util::statementToLocationString(*stmt->statement);
            }
            // std::cout << "Location string: " << location_str << std::endl;
            if (stmt->next == nullptr) {                
                THROW_ReachabilityError("Non-void function does not return a value: " + location_str);
            }
        }
        
        this->visit_children(stmt);
    }
}

void CfgReachabilityVisitor::operator()(CfgExpression *expr) {
    // TODO: implement
    reached.insert(expr->source_statement);
    
    // Empty expression in for/while evaluates to true
    if (expr->is_for_while && !expr->expression) {
        this->visit_child(expr->true_branch);
        return;    
    }

    if ( expr->is_for_while && isConstantExpression(*expr->expression) ) {
        Literal eval = evalConstantExpression(*expr->expression);
        if ( auto eval_bool = std::get_if<bool>(&eval) ) {
            if ( *eval_bool == true ) {
                this->visit_child(expr->true_branch);
                return;
            } else { // false
                this->visit_child(expr->false_branch);
                return;
            }
        }
    }

    this->visit_children(expr);
}
