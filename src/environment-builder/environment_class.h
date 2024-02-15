#include <unordered_map>
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <variant>

enum class VarType {
    BOOL,
    INT,
    SHORT,
    BYTE,
    CHAR,
    CLASS,
    INTERFACE,
    METHOD
};

class Environment;

typedef std::variant<std::monostate, bool, int64_t, char, std::string, Environment*> VarValue;

struct Variable {
    VarType type;
    VarValue value;
    bool is_array;
};

class Environment {
    protected:
        std::unique_ptr<Environment> parent = nullptr;
        std::unordered_map<std::string, Variable> variables;
        std::vector<Environment*> children;
    public:
        Environment(std::unique_ptr<Environment>& parent);
        // add variable to environment
        bool addVariable(const std::string& name, const Variable& variable);
        // lookup a variable in that environment and parent environments
        std::optional<Variable> lookupVariable(const std::string& name);
        // lookup all variables with matching name, in the current environment and parent environments
        std::optional<std::vector<Variable>> lookupVariables(const std::string& name);
        // add child environment to parent environment
        void addChild(Environment* child);
};

class PackageEnvironment: public Environment {
    public:
        PackageEnvironment(std::unique_ptr<Environment>& parent);
};

class ClassEnvironment: public Environment {
    std::string name;
    Environment* extends;
    std::vector<Environment*> implements;
    public:
        ClassEnvironment(
            std::unique_ptr<Environment>& parent,
            std::string& name,
            Environment*& extends,
            std::vector<Environment*>& implements
        );
};

class InterfaceEnvironment: public Environment {
    std::string name;
    std::vector<Environment*> extends;
    public:
        InterfaceEnvironment(
            std::unique_ptr<Environment>& parent,
            std::string& name,
            std::vector<Environment*>& extends
        );
};

class MethodEnvironment: public Environment {
    std::string name;
    std::string return_type;
    public:
        MethodEnvironment(
            std::unique_ptr<Environment>& parent,
            std::string& name,
            std::string& return_type
        );
};

class BlockEnvironment: public Environment {
    public:
        BlockEnvironment(std::unique_ptr<Environment>& parent);
};