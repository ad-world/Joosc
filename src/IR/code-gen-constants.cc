#include "code-gen-constants.h"
#include "utillities/util.h"

size_t CGConstants::next_method_id = 0;
std::unordered_map<MethodDeclarationObject*, std::string> CGConstants::method_labels = {};

size_t CGConstants::next_field_id = 0;
std::unordered_map<FieldDeclarationObject*, std::string> CGConstants::field_labels = {};

size_t CGConstants::next_variable_id = 0;
std::unordered_map<LocalVariableDeclarationObject*, std::string> CGConstants::variable_labels = {};

size_t CGConstants::next_parameter_id = 0;
std::unordered_map<FormalParameterDeclarationObject*, std::string> CGConstants::parameter_labels = {};
