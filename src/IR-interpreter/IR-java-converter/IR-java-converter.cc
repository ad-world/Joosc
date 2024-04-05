#include "IR-java-converter.h"

void IRJavaConverter::operator()(CompUnitIR &node) {
    result += "public class Main {\n";
    result += "\tpublic static void main(String[] args) {\n";

    visit_children(node);

    result += "\t\tCompUnit compUnit = new CompUnit(\"test\");\n";
    for (auto &func : functions) {
        result += "\t\tcompUnit.appendFunc(" + func + ");\n";
    }

    result += " {\n";
    result += "\tSimulator sim = new Simulator(compUnit);\n";
    result += "\tlong result = sim.call(\"test\")";
    result += "\tSystem.out.println(\"test results to: \" + result);\n";
    result += "}\n";
}

void IRJavaConverter::operator()(FuncDeclIR &node) {
    result += "\t\t Stmt " + node.getName() + "Body = \n";
    visit_children(node);
    result += ";";
    result += "\t\t FuncDecl func = new FuncDecl(\"" + node.getName() + "\"," + std::to_string(node.getNumParams()) + "," + node.getName() + "Body);\n";
}

void IRJavaConverter::operator()(BinOpIR &node) {
    result += "new BinOp(BinOp.OpType." + node.label() + ",";
    visit_children(node.getLeft());
    result += ",";
    visit_children(node.getRight());
    result += ")";
}

void IRJavaConverter::operator()(CallIR &node) {
    result += "new Call(";
    visit_children(node.getTarget());
    result += ",";
    for (auto &arg : node.getArgs()) {
        visit_children(arg);
        result += ",";
    }
    result += ")";
}

void IRJavaConverter::operator()(ConstIR &node) {
    result += "new Const(" + std::to_string(node.getValue()) + ")";
}

void IRJavaConverter::operator()(ESeqIR &node) {
    result += "new ESeq(";
    visit_children(node.getStmt());
    result += ",";
    visit_children(node.getExpr());
    result += ")";
}

void IRJavaConverter::operator()(MemIR &node) {
    result += "new Mem(";
    visit_children(node);
    result += ")";
}

void IRJavaConverter::operator()(NameIR &node) {
    result += "new Name(\"" + node.getName() + "\")";
}

void IRJavaConverter::operator()(TempIR &node) {
    result += "new Temp(" + node.getName() + ")";
}

void IRJavaConverter::operator()(CJumpIR &node) {
    result += "new CJump(";
    visit_children(node.getCondition());
    result += ", \"" + node.trueLabel() + "\", \"" + node.falseLabel() + "\")";
}

void IRJavaConverter::operator()(ExpIR &node) {
    result += "new Exp(";
    visit_children(node.getExpr());
    result += ")";
}

void IRJavaConverter::operator()(JumpIR &node) {
    result += "new Jump(";
    visit_children(node.getTarget());
    result += ")";
}

void IRJavaConverter::operator()(LabelIR &node) {
    result += "new Label(\"" + node.getName() + "\")";
}

void IRJavaConverter::operator()(MoveIR &node) {
    result += "new Move(";
    visit_children(node.getTarget());
    result += ",";
    visit_children(node.getSource());
    result += ")";
}

void IRJavaConverter::operator()(ReturnIR &node) {
    result += "new Return(";
    visit_children(node);
    result += ")";
}

void IRJavaConverter::operator()(SeqIR &node) {
    result += "new Seq(";
    for (auto &stmt : node.getStmts()) {
        visit_children(stmt);

        if (stmt != node.getStmts().back()) {
            result += ",";
        }
    }
    result += ")";
}

