#include "code-gen-constants.h"
#include "environment-builder/symboltableentry.h"
#include "utillities/util.h"

size_t CGConstants::next_method_id = 0;

std::unordered_map<MethodDeclarationObject*, std::string> CGConstants::method_labels = {};

std::string CGConstants::uniqueMethodLabel(MethodDeclarationObject* method) {
    if (!method_labels.count(method)) {
        std::string label = "l" + method->identifier + "l_l" + std::to_string(next_method_id) + "l";
        method_labels[method] = label;
    }

    return method_labels[method];
}
