#include <unordered_map>
#include <string>
#include <memory>
#include <optional>
#include <vector>
#include <variant>

enum PackageType {
    CLASS,
    INTERFACE
};

typedef std::variant<std::monostate, bool, int64_t, char, std::string, PackageType> VariableType;

enum Modifier {
    PUBLIC,
    PROTECTED,
    ABSTRACT,
    STATIC,
    NATIVE,
    FINAL
};

struct Variable {
    std::string type;
    VariableType value;
    bool is_array;
};

class RootEnvironment {
    protected:
        std::unique_ptr<RootEnvironment> parent = nullptr;
        std::unordered_map<std::string, Variable> variables;
    public:
        RootEnvironment(std::unique_ptr<RootEnvironment>& parent);
        // add variable to environment
        void addVariable(const std::string& name, const Variable& variable);
        // lookup a variable in that environment and parent environments
        std::optional<Variable> lookupVariable(const std::string& name);
};

class PackageEnvironment: public RootEnvironment {
    public:
        PackageEnvironment(std::unique_ptr<RootEnvironment>& parent);
};

class ClassEnvironment: public RootEnvironment {
    std::vector<Modifier> modifiers;
    public:
        ClassEnvironment(
            std::unique_ptr<RootEnvironment>& parent,
            std::vector<Modifier>& modifier
        );
};

class InterfaceEnvironment: public RootEnvironment {
    std::vector<Modifier> modifiers;
    public:
        InterfaceEnvironment(
            std::unique_ptr<RootEnvironment>& parent,
            std::vector<Modifier>& modifiers
        );
};

class MethodEnvironment: public RootEnvironment {
    std::vector<Modifier> modifiers;
    std::string name;
    std::string return_type;
    public:
        MethodEnvironment(
            std::unique_ptr<RootEnvironment>& parent,
            std::vector<Modifier>& modifiers,
            std::string& name,
            std::string& return_type
        );
};

class BlockEnvironment: public RootEnvironment {
    public:
        BlockEnvironment(std::unique_ptr<RootEnvironment>& parent);
};