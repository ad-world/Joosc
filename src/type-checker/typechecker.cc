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

ClassDeclarationObject* TypeChecker::getStringClass(LinkedType &link) {
    if(link.isReferenceType()) {
        if(link.getIfNonArrayIsClass() == default_package->findClassDeclaration("java.lang.String")) {
            return link.getIfNonArrayIsClass();
        }
    }
    return nullptr;
}

void TypeChecker::operator()(CompilationUnit &node) {
    this->compilation_unit_namespace = node.cu_namespace;
    visit_children(node);
}

void TypeChecker::operator()(ClassDeclaration &node) {
    this->current_class = node.environment;
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

// Return whether the class_with_field has a field with simple name field_simple_name
// that is accessible by current_class
//
// Returns pointer to FieldDeclarationObject if accessible, and nullptr otherwise
FieldDeclarationObject* checkIfFieldIsAccessible(
    ClassDeclarationObject* current_class,
    ClassDeclarationObject* class_with_field,
    std::string& field_simple_name,
    bool must_be_static = false
) {
    // First, see if the field even exists
    auto possible_field = class_with_field->accessible_fields[field_simple_name];
    if (!possible_field) { return nullptr; }

    // Refactor if this works later
    ClassDeclarationObject* class_that_declared_field = possible_field->containing_class;

    // Second, if the field must be static, ensure it is static
    if (must_be_static && !possible_field->ast_reference->hasModifier(Modifier::STATIC)) {
        return nullptr;
    }

    // Third, if the field is protected, the current_class must be a subclass or in the same package of class_with_field
    if (possible_field->ast_reference->hasModifier(Modifier::PROTECTED)) {
        if (
            !current_class->isSubType(class_with_field) && 
            !(current_class->package_contained_in == class_that_declared_field->package_contained_in)
        ) {
            std::cerr << "Protected field not accessible\n";
            return nullptr;
        }
    }

    return possible_field;
}

// TODO : ensure public/private fields are accessible in correct place
void TypeChecker::operator()(QualifiedIdentifier &qid) {
    switch (qid.getClassification()) {
        case EXPRESSION_NAME: 
            if (qid.isSimple()) {
                std::string& name = qid.identifiers.back().name;
                // 1. Look up in local vars scope
                if (current_method) {
                    auto possible_var = current_method->scope_manager.lookupDeclaredVariable(name);
                    // auto possible_var = current_method->scope_manager.lookupVariable(name);
                    if (possible_var) {
                        qid.link = possible_var->type;
                        return;
                    }
                }
                // 2. Look up in parameters
                if (current_method) {
                    auto possible_param 
                        = current_method->parameters->lookupUniqueSymbol<FormalParameterDeclarationObject>(name);
                    if (possible_param) {
                        qid.link = possible_param->type;
                        return;
                    }
                }
                // 3. Look up in fields
                auto decl_type = compilation_unit_namespace.getDeclaredType();
                if (auto cls = std::get_if<ClassDeclarationObject*>(&decl_type)) {
                    if (auto possible_field = checkIfFieldIsAccessible(current_class, *cls, name)) {
                        qid.link = possible_field->type;
                        return;
                    }
                }
                THROW_TypeCheckerError("Undefined reference to " + qid.getQualifiedName());
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
                        if (auto cls = std::get_if<ClassDeclarationObject*>(&cls_lookup)) {
                            if (auto possible_field = checkIfFieldIsAccessible(current_class, *cls, id, true)) {
                                qid.link = possible_field->type;
                                return;
                            }
                            THROW_TypeCheckerError(
                                "Class " + Q.getQualifiedName() + " does not have an accessible field named " + id
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
                            if (auto possible_field = checkIfFieldIsAccessible(current_class, class_type, id)) {
                                qid.link = possible_field->type;
                                return;
                            }
                            THROW_TypeCheckerError(
                                "No accessible field " + id + " in " + Q.getQualifiedName() + " from class " + current_class->identifier
                            );
                        }
                        THROW_TypeCheckerError(
                                "Interface ExpressionName " + Q.getQualifiedName() + " precedes ExpressionName " + id);
                    }
                    case METHOD_NAME:
                        THROW_CompilerError(
                            "MethodName precedes ExpressionName in " 
                            + qid.getQualifiedName() + " which should never happen");
                }
                THROW_TypeCheckerError("Undefined reference to " + qid.getQualifiedName());
            }
        default:
            // Not an expression
            return;
    }
}

bool checkAssignability(LinkedType& linkedType1, LinkedType& linkedType2, PackageDeclarationObject* default_package) {
    if(linkedType1.isPrimitive()) {
        auto typeEnum1 = std::get<PrimitiveType>(linkedType1.linked_type);
        if(linkedType2.isPrimitive()) {
            auto typeEnum2 = std::get<PrimitiveType>(linkedType2.linked_type);
            if(typeEnum1 == typeEnum2) {
                return true;
            }
            else if(((typeEnum1 == PrimitiveType::INT && linkedType2.isNumeric()) || 
                    (typeEnum1 == PrimitiveType::SHORT && typeEnum2 == PrimitiveType::BYTE)) && 
                    !linkedType1.is_array && !linkedType2.is_array) {
                return true;
            }
        }
    }
    else {
        if(linkedType2.isNull() || linkedType1.isSubType(linkedType2, default_package)) {
            return true;
        }
    }
    return false;
}

void TypeChecker::operator()(Assignment &node) {
    this->visit_children(node);

    LinkedType linkedType1 = getLink(node.assigned_to);
    LinkedType linkedType2 = getLink(node.assigned_from);

    if(checkAssignability(linkedType1, linkedType2, default_package))
    {
        node.link.linked_type = linkedType1.linked_type;
    }
    else {
        THROW_TypeCheckerError("Invalid type for assignment operation");
    }
}

void TypeChecker::operator()(InfixExpression &node) {
    this->visit_children(node);

    LinkedType linkedType1 = getLink(node.expression1);
    LinkedType linkedType2 = getLink(node.expression1);

    switch (node.op) {
        case InfixOperator::PLUS:
            if((getStringClass(linkedType1) && !linkedType2.isVoid()) || (getStringClass(linkedType2) && !linkedType1.isVoid())) {
                node.link = LinkedType(NonArrayLinkedType(default_package->findClassDeclaration("java.lang.String")));
            }
        case InfixOperator::MINUS:
        case InfixOperator::MULTIPLY:
        case InfixOperator::DIVIDE:
        case InfixOperator::MODULO:
            if(linkedType1.isNumeric() && linkedType2.isNumeric()) {
                node.link = LinkedType(PrimitiveType::INT);
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
                node.link = LinkedType(PrimitiveType::BOOLEAN);
            }
            else {
                THROW_TypeCheckerError("Invalid type for comparison operation");
            }
            break;
        case InfixOperator::BOOLEAN_EQUAL:
        case InfixOperator::BOOLEAN_NOT_EQUAL:
            if(linkedType1.isSubType(linkedType2, default_package) || linkedType2.isSubType(linkedType1, default_package) ||
                (linkedType1.isNull() && (linkedType2.isReferenceType() || linkedType2.is_array))  
                || (linkedType2.isNull() && (linkedType1.isReferenceType() || linkedType1.is_array))
                || (linkedType1.isNumeric() && linkedType2.isNumeric())) {
                node.link = LinkedType(PrimitiveType::BOOLEAN);
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
                node.link = LinkedType(PrimitiveType::BOOLEAN);
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
    if(node.qualified_this.get())
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

bool isFinal(LinkedType type) {
    if(type.isReferenceType()) {
        ClassDeclarationObject* class_decl = type.getIfNonArrayIsClass();
        if(class_decl != nullptr) {
            return class_decl->ast_reference->hasModifier(Modifier::FINAL);
        }
    }
    return false;
}

bool checkCastability(LinkedType type1, LinkedType type2, PackageDeclarationObject* default_package) {
    if(type1.isNumeric() && type2.isNumeric() && !type1.is_array && !type2.is_array) {
        return true;
    }
    else if((type2.getIfNonArrayIsInterface() && (type1.getIfNonArrayIsInterface() || !isFinal(type1))) 
            || (type1.getIfNonArrayIsInterface() && (type2.getIfNonArrayIsInterface() || !isFinal(type2)))) {
        return true;
    }
    else if(type1.isSubType(type2, default_package)) {
        return true;
    }
    return false;
}

void TypeChecker::operator()(CastExpression &node) {
    this->visit_children(node);

    LinkedType type = node.type.get()->link;
    LinkedType expression = getLink(node.expression);

    if(checkCastability(type, expression, default_package)) {
        node.link = type;
    }
    else {
        THROW_TypeCheckerError("Invalid type for CastExpression");
    }
}

void TypeChecker::operator()(InstanceOfExpression &node) {
    this->visit_children(node);

    LinkedType expression = getLink(node.expression);
    LinkedType type = node.type.get()->link;
    if(type.isReferenceType() && checkCastability(expression, type, default_package)) {
        node.link = LinkedType(PrimitiveType::BOOLEAN);
    }
    else {
        THROW_TypeCheckerError("Invalid type for InstanceOfExpression");
    }
}

TypeChecker::TypeChecker(PackageDeclarationObject &default_package) 
    : default_package{&default_package}, current_method{nullptr} {}
