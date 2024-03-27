#include "IR/ir_visitor.h"
#include <unordered_map>
#include "IR/ir.h"

class MapsBuilder : public IRSkipVisitor {
    std::unordered_map<std::string, int> nameToIndex;
    std::unordered_map<int, IR_PTR> indexToNode;

    int index;
public:
    using IRSkipVisitor::visit;
    MapsBuilder();
    std::unordered_map<std::string, int> &getNameToIndex();
    std::unordered_map<int, IR_PTR> &getIndexToNode();

    void addNode(IR_PTR node);
    void addNameToCurrentIndex(std::string name);

    // CompUnitIR
    void operator()(CompUnitIR &node) override;

    // FuncDeclIR
    void operator()(FuncDeclIR &node) override;

    // ExpressionIRs
    void operator()(BinOpIR &node) override;
    void operator()(CallIR &node) override;
    void operator()(ConstIR &node) override;
    void operator()(ESeqIR &node) override;
    void operator()(MemIR &node) override;
    void operator()(NameIR &node) override;
    void operator()(TempIR &node) override;

    // StatementIRs
    void operator()(CJumpIR &node) override;
    void operator()(ExpIR &node) override;
    void operator()(JumpIR &node) override;
    void operator()(LabelIR &node) override;
    void operator()(MoveIR &node) override;
    void operator()(ReturnIR &node) override;
    void operator()(SeqIR &node) override;
};