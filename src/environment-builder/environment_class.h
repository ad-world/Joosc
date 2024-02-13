#include <unordered_map>
#include <string>
#include <memory>
#include <optional>
#include <variant>

typedef std::variant<bool, int, char, std::byte, short, std::string> VariableType;

struct Variable {
    std::string type;
    VariableType value;
    bool is_array;
};

class RootEnvironment {
    protected:
        std::unique_ptr<RootEnvironment> parent;
        std::unordered_map<std::string, Variable> variables;
    public:
        RootEnvironment(std::unique_ptr<RootEnvironment> parent = nullptr);
        // add variable to environment
        void addVariable(const std::string& name, const Variable& variable);
        // lookup a variable in that environment and parent environments
        std::optional<Variable> lookupVariable(const std::string& name);
};

class ClassEnvironment: public RootEnvironment {
    public:
        ClassEnvironment(std::unique_ptr<RootEnvironment> parent);
};

class MethodEnvironment: public RootEnvironment {
    public:
        MethodEnvironment(std::unique_ptr<RootEnvironment> parent);
};

class BlockEnvironment: public RootEnvironment {
    public:
        BlockEnvironment(std::unique_ptr<RootEnvironment> parent);
};