#include "interface-extender.h"

void InterfaceExtender::operator()(InterfaceDeclaration &node) {
}

void InterfaceExtender::visit(AstNodeVariant &node) {
    std::visit(*this, node);
}
