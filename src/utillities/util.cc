#include "add-location/add-location.h"
#include "util.h"
PackageDeclarationObject* Util::root_package = nullptr;
std::vector<AstNodeVariant>* Util::linked_asts = nullptr;

std::string Util::locationToString(yy::location &loc) {
    return AddLocation::getString(loc);
}
