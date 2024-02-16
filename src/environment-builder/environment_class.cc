#include "environment_class.h"

Environment::Environment(
    std::unique_ptr<Environment>& parent,
    AstNode*& astNode
) :
    parent{std::move(parent)},
    astNode{std::move(astNode)}
{}

// returns true if successful and false if variable name exists already but is not of type class/interface/method
bool Environment::addVariable(const std::string& name, const Variable& variable) {
    std::optional<std::vector<Variable>> exists = lookupVariables(name);
    if(exists.has_value()) {
        for(const Variable& var: exists.value()) {
            if(var.type != VarType::CLASS || var.type != VarType::INTERFACE || var.type != VarType::METHOD) {
                return false;
            }
        }
    }
    variables[name].push_back(variable);
    return true;
};

std::optional<std::vector<Variable>> Environment::lookupVariable(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    } else if (parent) {
        return parent->lookupVariable(name);
    }
    return std::nullopt;
};

std::optional<std::vector<Variable>> Environment::lookupVariables(const std::string& name) {
    std::vector<Variable> foundVariables;
    for (auto env = this; env != nullptr; env = env->parent.get()) {
        auto it = env->variables.find(name);
        if (it != env->variables.end()) {
            for(const auto& var: it->second) {
                foundVariables.push_back(var);
            }
        }
    }
    if (!foundVariables.empty()) {
        return foundVariables;
    } else {
        return std::nullopt;
    }
};

void Environment::addChild(Environment*& child) {
    children.push_back(child);
}

PackageEnvironment::PackageEnvironment(
    std::unique_ptr<Environment>& parent,
    AstNode*& astNode
) :
    Environment(parent, astNode)
{}

ClassEnvironment::ClassEnvironment(
    std::unique_ptr<Environment>& parent,
    AstNode*& astNode,
    std::string& name,
    Environment*& extends,
    std::vector<Environment*>& implements
) :
    Environment(parent, astNode),
    name{std::move(name)},
    extends{std::move(extends)},
    implements{std::move(implements)}
{}

InterfaceEnvironment::InterfaceEnvironment(
    std::unique_ptr<Environment>& parent,
    AstNode*& astNode,
    std::string& name,
    std::vector<Environment*>& extends
) :
    Environment(parent, astNode),
    name{std::move(name)},
    extends{std::move(extends)}
{}

MethodEnvironment::MethodEnvironment(
    std::unique_ptr<Environment>& parent,
    AstNode*& astNode,
    std::string& name,
    std::string& return_type
) :
    Environment(parent, astNode),
    name{std::move(name)},
    return_type{std::move(return_type)}
{}

BlockEnvironment::BlockEnvironment(
    std::unique_ptr<Environment>& parent,
    AstNode*& astNode
) :
    Environment(parent, astNode)
{}
