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
};

typedef std::variant<std::monostate, bool, int64_t, char, std::string, RootEnvironment*> VarValue;

struct Variable {
    VarType type;
    VarValue value;
    bool is_array;
};

class RootEnvironment {
    protected:
        std::unique_ptr<RootEnvironment> parent = nullptr;
        std::unordered_map<std::string, Variable> variables;
        std::vector<RootEnvironment*> children;
    public:
        RootEnvironment(std::unique_ptr<RootEnvironment>& parent);
        // add variable to environment
        void addVariable(const std::string& name, const Variable& variable);
        // lookup a variable in that environment and parent environments
        std::optional<Variable> lookupVariable(const std::string& name);
        // add child environment to parent environment
        void addChild(RootEnvironment* child);
};

class PackageEnvironment: public RootEnvironment {
    public:
        PackageEnvironment(std::unique_ptr<RootEnvironment>& parent);
};

class ClassEnvironment: public RootEnvironment {
    std::string name;
    RootEnvironment* extends;
    std::vector<RootEnvironment*> implements;
    public:
        ClassEnvironment(
            std::unique_ptr<RootEnvironment>& parent,
            std::string& name,
            RootEnvironment*& extends,
            std::vector<RootEnvironment*>& implements
        );
};

class InterfaceEnvironment: public RootEnvironment {
    std::string name;
    std::vector<RootEnvironment*> extends;
    public:
        InterfaceEnvironment(
            std::unique_ptr<RootEnvironment>& parent,
            std::string& name,
            std::vector<RootEnvironment*>& extends
        );
};

class MethodEnvironment: public RootEnvironment {
    std::string name;
    std::string return_type;
    public:
        MethodEnvironment(
            std::unique_ptr<RootEnvironment>& parent,
            std::string& name,
            std::string& return_type
        );
};

class BlockEnvironment: public RootEnvironment {
    public:
        BlockEnvironment(std::unique_ptr<RootEnvironment>& parent);
};