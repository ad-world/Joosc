#include <memory>
#include <vector>
#include "inheritance-ast/ast.h"
#include "inheritance-ast/visitor.h"

namespace Inheritance {

void BinaryExpression::accept(AstVisitor &v) { v.visit(*this); };
void UnaryExpression::accept(AstVisitor &v) { v.visit(*this); };
void Integer::accept(AstVisitor &v) { v.visit(*this); };

void IfStatement::accept(AstVisitor &v) { v.visit(*this); };
void ExpressionStatement::accept(AstVisitor &v) { v.visit(*this); };

void CompilationUnit::accept(AstVisitor &v) { v.visit(*this); };

}
