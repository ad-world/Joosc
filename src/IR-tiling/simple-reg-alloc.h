#include <string>
#include <unordered_map>
#include <list>
#include <vector>

// Do brainless register allocation storing all the abstract registers on stack
// Using a map to store the stack offsets for each abstract register
class RegAlloc {
    std::unordered_map<std::string, int> reg_offsets;
    int stack_offset = 0;

  public:
    // Allocate a new abstract register
    void allocReg(std::string reg);

    // Get the stack offset for an abstract register
    int getOffset(std::string reg);

    // Prologue
    // Order of register assignments is EAX, ECX, EDX
    std::vector<std::string> prologue(std::vector<std::string> abstract_regs);

    // Epilogue
    std::vector<std::string> epilogue(std::vector<std::string> abstract_regs);

    // Conversion from abstract register to concrete register
    std::vector<std::string> concreteAsm(std::list<std::string> asm_instructions);
};