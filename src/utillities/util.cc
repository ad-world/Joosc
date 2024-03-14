#include "add-location/add-location.h"
#include "util.h"
PackageDeclarationObject* Util::root_package = nullptr;
std::vector<AstNodeVariant>* Util::linked_asts = nullptr;

std::string Util::locationToString(yy::location &loc) {
    return AddLocation::getString(loc);
}


std::string Util::statementToLocationString(Statement &stmt) {
    yy::location *location = nullptr;

    std::visit(util::overload{
        [&](IfThenStatement &node) -> void {
            location = &node.location;
        },
        [&](IfThenElseStatement &node) -> void {
            location = &node.location;
        },
        [&](WhileStatement &node) -> void {
            location = &node.location;
        },
        [&](ForStatement &node) -> void {
            location = &node.location;
        },
        [&](Block &node) -> void {
            location = &node.location;
        },
        [&](ExpressionStatement &node) -> void {
            std::visit(util::overload{
                [&](auto &node) -> void {
                    location = &node.location;
                }
            }, node);
        },
        [&](std::monostate &node) -> void {},
        // Default case for all other statement types
        [&](auto &node) -> void {
            location = &node.location;
        },
     }, stmt);

    std::string location_str;
    if ( location ) {
        location_str = Util::locationToString(*location);
    }
    return location_str;
}
