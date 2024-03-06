#include "type-checker/typechecker.h"
#include "utillities/util.h"
#include "exceptions/exceptions.h"
#include "environment-builder/symboltable.h"

LinkedType TypeChecker::getLink(Expression &node) {
    return std::visit(util::overload {
        // Literal handled seperately as it is not a class
        [&] (Literal expr_type) -> LinkedType {
            return std::visit(util::overload {
                [&] (int64_t literal_type) -> LinkedType {
                    return LinkedType(PrimitiveType::INT);
                },
                [&] (bool literal_type) -> LinkedType {
                    return LinkedType(PrimitiveType::BOOLEAN);
                },
                [&] (char literal_type) -> LinkedType {
                    return LinkedType(PrimitiveType::CHAR);
                },
                [&] (std::string& literal_type) -> LinkedType {
                    NonArrayLinkedType string_type = default_package->findClassDeclaration("java.lang.String");
                    return LinkedType(string_type);
                },
                [&] (std::nullptr_t literal_type) -> LinkedType {
                    return LinkedType(PrimitiveType::NULL_T);
                },
            }, expr_type);
        },
        // Any non-literal subexpression
        [] (auto& expr_type) -> LinkedType {
            return expr_type.link;
        }
    }, node);
}

LinkedType TypeChecker::getLink(std::unique_ptr<Expression>& node_ptr) {
    return getLink(*node_ptr);
}

void TypeChecker::operator()(CompilationUnit &node) {
    this->compilation_unit_namespace = node.cu_namespace;
    visit_children(node);
}

void TypeChecker::operator()(MethodDeclaration &node) {
    current_method = node.environment;
    current_method->scope_manager.closeAllScopes();
    visit_children(node);
    current_method->scope_manager.closeAllScopes();
    current_method = nullptr;
}

void TypeChecker::operator()(Block &node) {
    current_method->scope_manager.openScope(node.scope_id);
    visit_children(node);
    current_method->scope_manager.closeScope(node.scope_id);
}

void TypeChecker::operator()(LocalVariableDeclaration &node) {
    current_method->scope_manager.declareVariable(node.variable_declarator->variable_name->name);
    visit_children(node);
}

// TODO : ensure public/private fields are accessible in correct place
void TypeChecker::operator()(QualifiedIdentifier &qid) {
    switch (qid.getClassification()) {
        case EXPRESSION_NAME: 
            if (qid.isSimple()) {
                std::string& name = qid.identifiers.back().name;
                // 1. Look up in local vars scope
                auto possible_var = current_method->scope_manager.lookupDeclaredVariable(name);
                if (possible_var) {
                    qid.link = possible_var->type;
                    return;
                }
                // 2. Look up in parameters
                auto possible_param 
                    = current_method->parameters->lookupUniqueSymbol<FormalParameterDeclarationObject>(name);
                if (possible_param) {
                    qid.link = possible_param->type;
                    return;
                }
                // 3. Look up in fields
                auto decl_type = compilation_unit_namespace.getDeclaredType();
                if (auto cls = std::get_if<ClassDeclarationObject*>(&decl_type)) {
                    if (auto possible_field = (*cls)->fields->lookupUniqueSymbol<FieldDeclarationObject>(name)) {
                        qid.link = possible_field->type;
                        return;
                    }
                }
            } else {
                // 6.5.6.2: Expression name is of the form Q.id
                QualifiedIdentifier Q = qid.getQualifiedIdentifierWithoutLast();
                std::string& id = qid.identifiers.back().name;
                switch (Q.getClassification()) {
                    case PACKAGE_NAME:
                        THROW_TypeCheckerError("PackageName precedes ExpressionName in " + qid.getQualifiedName());
                    case TYPE_NAME: {
                        // Q must be a class, and id must be a static field 
                        auto cls_lookup = compilation_unit_namespace.lookupQualifiedType(Q);
                        auto cls = std::get_if<ClassDeclarationObject*>(&cls_lookup);
                        if (cls) {
                            if (auto possible_field = (*cls)->fields->lookupUniqueSymbol<FieldDeclarationObject>(id)) {
                                if (!possible_field->ast_reference->hasModifier(Modifier::STATIC)) {
                                    THROW_TypeCheckerError("Instance field referenced from ClassName in " + qid.getQualifiedName());
                                }
                                qid.link = possible_field->type;
                                return;
                            }
                            THROW_TypeCheckerError(
                                "Class " + Q.getQualifiedName() + " does not have a field named " + id
                            );
                        }
                        // Q is not a class
                        THROW_TypeCheckerError(
                            "Attempted to access field " + id + " in interface " + Q.getQualifiedName());
                    }
                    case EXPRESSION_NAME: {
                        // Must determine type T of Q
                        this->operator()(Q);
                        LinkedType T = Q.link;
                        if (!T.isReferenceType()) {
                            THROW_TypeCheckerError(
                                "Primitive ExpressionName precedes ExpressionName in " + Q.getQualifiedName());
                        }
                        if (T.is_array && id == "length") {
                            // Special case: "length" field of an array
                            qid.link = LinkedType(PrimitiveType::INT);
                            return;
                        }
                        if (auto class_type = T.getIfNonArrayIsClass()) {
                            if (auto possible_field 
                                    = class_type->fields->lookupUniqueSymbol<FieldDeclarationObject>(id)) {
                                qid.link = possible_field->type;
                                return;
                            }
                        }
                        THROW_TypeCheckerError(
                                "Interface ExpressionName precedes ExpressionName in " + Q.getQualifiedName());
                    }
                    case METHOD_NAME:
                        THROW_CompilerError(
                            "MethodName precedes ExpressionName in " 
                            + qid.getQualifiedName() + " which should never happen");
                }
            }
            THROW_TypeCheckerError("Undefined reference to " + qid.getQualifiedName());
        default:
            // Not an expression
            return;
    }
}


void TypeChecker::operator()(Assignment &node) {
    this->visit_children(node);
}

void TypeChecker::operator()(InfixExpression &node) {
    this->visit_children(node);

    LinkedType linkedType1 = getLink(node.expression1);
    LinkedType linkedType2 = getLink(node.expression1);
    if(linkedType1.is_array || linkedType2.is_array) {
        THROW_TypeCheckerError("Infix operators are not defined for array types");
    }

    switch (node.op) {
        case InfixOperator::PLUS:
            if(linkedType1.isNumeric() && linkedType2.isNumeric()) {
                node.link.linked_type = PrimitiveType::INT;
            }
            // else if((linkedType1.isString() && !linkedType2.isVoid()) || (linkedType2.isString() && !linkedType1.isVoid())) {
            // }
            else {
                THROW_TypeCheckerError("Invalid type for arithmetic operation");
            }
            break;
        case InfixOperator::MINUS:
        case InfixOperator::MULTIPLY:
        case InfixOperator::DIVIDE:
        case InfixOperator::MODULO:
            if(linkedType1.isNumeric() && linkedType2.isNumeric()) {
                node.link.linked_type = PrimitiveType::INT;
            }
            else {
                THROW_TypeCheckerError("Invalid type for arithmetic operation");
            }
            break;
        case InfixOperator::LESS_THAN:
        case InfixOperator::GREATER_THAN:
        case InfixOperator::LESS_THAN_EQUAL:
        case InfixOperator::GREATER_THAN_EQUAL:
            if(linkedType1.isNumeric() && linkedType2.isNumeric()) {
                node.link.linked_type = PrimitiveType::BOOLEAN;
            }
            else {
                THROW_TypeCheckerError("Invalid type for comparison operation");
            }
            break;
        case InfixOperator::BOOLEAN_EQUAL:
        case InfixOperator::BOOLEAN_NOT_EQUAL:
            if(linkedType1 == linkedType2 || (linkedType1.isNull() && (!linkedType2.isPrimitive() || linkedType2.is_array)) 
                || (linkedType2.isNull() && (!linkedType1.isPrimitive() || linkedType1.is_array))) {
                node.link.linked_type = PrimitiveType::BOOLEAN; // Might need to add check for subclasses and base class comparison
            }
            else {
                THROW_TypeCheckerError("Invalid type for comparison operation");
            }
            break;
        case InfixOperator::BOOLEAN_AND:
        case InfixOperator::BOOLEAN_OR:
        case InfixOperator::EAGER_AND:
        case InfixOperator::EAGER_OR:
            if(linkedType1.isBoolean() && linkedType2.isBoolean()) {
                node.link.linked_type = PrimitiveType::BOOLEAN;
            }
            else {
                THROW_TypeCheckerError("Invalid type for boolean operation");
            }
            break;
        default:
            THROW_TypeCheckerError("Invalid type for InfixExpression");
    }
}

void TypeChecker::operator()(PrefixExpression &node) {
    this->visit_children(node);

    LinkedType linkedType = getLink(node.expression);
    if(linkedType.is_array) {
        THROW_TypeCheckerError("Prefix operators are not defined for array types");
    }

    switch (node.op) {
        case PrefixOperator::MINUS:
            if(linkedType.isNumeric()) {
                node.link.linked_type = linkedType.linked_type;
            }
            else {
                THROW_TypeCheckerError("Invalid type for arithmetic operation");
            }
            break;
        case PrefixOperator::NEGATE:
            if(linkedType.isBoolean()) {
                node.link.linked_type = linkedType.linked_type;
            }
            else {
                THROW_TypeCheckerError("Invalid type for boolean operation");
            }
            break;
        default:
            THROW_TypeCheckerError("Invalid type for PrefixExpression");
    }
}

void TypeChecker::operator()(QualifiedThis &node) {
    this->visit_children(node);
    node.link = node.qualified_this.get()->link;
}

void TypeChecker::operator()(ArrayCreationExpression &node) {
    this->visit_children(node);

    LinkedType linkedType = getLink(node.expression);
    if(linkedType.isNumeric()) {
        node.link.linked_type = linkedType.linked_type;
        node.link.is_array = true;
    }
    else {
        THROW_TypeCheckerError("Invalid type for ArrayCreationExpression");
    }
}

void TypeChecker::operator()(ClassInstanceCreationExpression &node) {
    this->visit_children(node);
    node.link = node.linked_class_type;
}

void TypeChecker::operator()(FieldAccess &node) {
    this->visit_children(node);
}

void TypeChecker::operator()(ArrayAccess &node) {
    this->visit_children(node);

    LinkedType typeArray = getLink(node.array);
    LinkedType typeSelector = getLink(node.selector);
    if(typeArray.is_array && typeSelector.isNumeric()) {
        node.link.linked_type = typeArray.linked_type;
    }
    else {
        THROW_TypeCheckerError("Invalid type for ArrayAccess");
    }
}

void TypeChecker::operator()(MethodInvocation &node) {
    this->visit_children(node);
}

void TypeChecker::operator()(InstanceOfExpression &node) {
    this->visit_children(node);
}

void TypeChecker::operator()(CastExpression &node) {
    this->visit_children(node);
}

TypeChecker::TypeChecker(PackageDeclarationObject &default_package) 
    : default_package{&default_package}, current_method{nullptr} {}
