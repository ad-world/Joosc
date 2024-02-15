#include "environment_class.h"

RootEnvironment::RootEnvironment(std::unique_ptr<RootEnvironment>& parent): parent{std::move(parent)} {}

// returns true if successful and false if variable name exists already but is not of type class/interface/method
bool RootEnvironment::addVariable(const std::string& name, const Variable& variable) {
    std::optional<std::vector<Variable>> exists = lookupVariables(name);
    if(exists.has_value()) {
        for(const Variable& var: exists.value()) {
            if(var.type != VarType::CLASS || var.type != VarType::INTERFACE || var.type != VarType::METHOD) {
                return false;
            }
        }
    }
    variables[name] = variable;
    return true;
};

std::optional<Variable> RootEnvironment::lookupVariable(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    } else if (parent) {
        return parent->lookupVariable(name);
    }
    return std::nullopt;
};

std::optional<std::vector<Variable>> RootEnvironment::lookupVariables(const std::string& name) {
    std::vector<Variable> foundVariables;
    for (auto env = this; env != nullptr; env = env->parent.get()) {
        auto it = env->variables.find(name);
        if (it != env->variables.end()) {
            foundVariables.push_back(it->second);
        }
    }
    if (!foundVariables.empty()) {
        return foundVariables;
    } else {
        return std::nullopt;
    }
};

void RootEnvironment::addChild(RootEnvironment* child) {
    children.push_back(child);
}

PackageEnvironment::PackageEnvironment(std::unique_ptr<RootEnvironment>& parent): RootEnvironment(parent) {}

ClassEnvironment::ClassEnvironment(
    std::unique_ptr<RootEnvironment>& parent,
    std::string& name,
    RootEnvironment*& extends,
    std::vector<RootEnvironment*>& implements
) :
    RootEnvironment(parent),
    name{std::move(name)},
    extends{std::move(extends)},
    implements{std::move(implements)}
{}

InterfaceEnvironment::InterfaceEnvironment(
    std::unique_ptr<RootEnvironment>& parent,
    std::string& name,
    std::vector<RootEnvironment*>& extends
) :
    RootEnvironment(parent),
    name{std::move(name)},
    extends{std::move(extends)}
{}

MethodEnvironment::MethodEnvironment(
    std::unique_ptr<RootEnvironment>& parent,
    std::string& name,
    std::string& return_type
) :
    RootEnvironment(parent),
    name{std::move(name)},
    return_type{std::move(return_type)}
{}

BlockEnvironment::BlockEnvironment(std::unique_ptr<RootEnvironment>& parent): RootEnvironment(parent) {}