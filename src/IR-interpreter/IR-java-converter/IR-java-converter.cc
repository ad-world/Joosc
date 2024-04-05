#include "IR-java-converter.h"

IRJavaConverter::IRJavaConverter() {
    num_tabs = 0;
    result = "";
};

std::string IRJavaConverter::getFunctionName(std::string functionName) {
    int last_dot = functionName.find_last_of('.');
    if (last_dot != std::string::npos) {
        std::string short_func = functionName.substr(last_dot + 1);
        return short_func;
    }

    return functionName;
};

void IRJavaConverter::operator()(CompUnitIR &node) {
    result += "package joosc.ir.interpret;\nimport joosc.ir.ast.*;\nimport joosc.ir.visit.CheckCanonicalIRVisitor;\n\n";
    result += "public class Main {";
    num_tabs += 1;
    result += getNewlineAndTabString() + "public static void main(String[] args) {";
    visit_children(node);

    num_tabs += 1;

    result += getNewlineAndTabString() + "CompUnit compUnit = new CompUnit(\"test\");";
    for (auto &func : functions) {
        result += getNewlineAndTabString() + "compUnit.appendFunc(" + func + ");";
    }

    result += getNewlineAndTabString() + "{";
    num_tabs += 1;
    result += getNewlineAndTabString() + "Simulator sim = new Simulator(compUnit);";
    result += getNewlineAndTabString() + "long result = sim.call(\"test\");";
    result += getNewlineAndTabString() + "System.out.println(\"Java Interpretation of IR results to: \" + result);";
    num_tabs -= 1;
    result += getNewlineAndTabString() + "}";
    result += getNewlineAndTabString() + "{";
    num_tabs += 1;
    result += getNewlineAndTabString() + "CheckCanonicalIRVisitor cv = new CheckCanonicalIRVisitor();";
    result += getNewlineAndTabString() + "System.out.print(\"Canonical? \");";
    result += getNewlineAndTabString() + "System.out.println(cv.visit(compUnit));";
    num_tabs -= 1;
    result += getNewlineAndTabString() + "}";
    num_tabs -= 1;
    result += getNewlineAndTabString() + "}";
    num_tabs -= 1;
    result += getNewlineAndTabString() + "}";
}

void IRJavaConverter::operator()(FuncDeclIR &node) {

    std::string function_name = node.getName() + "Func";
    functions.push_back(function_name); 

    num_tabs += 1;
    result += getNewlineAndTabString() + "Stmt " + node.getName() + "Body = ";
    visit_children(node);
    result += ";";
    result += getNewlineAndTabString() + "FuncDecl " + function_name + " = new FuncDecl(\"" + node.getName() + "\"," + std::to_string(node.getNumParams()) + "," + node.getName() + "Body);\n";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(BinOpIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new BinOp(BinOp.OpType." + node.label() + ",";
    visit_children(node.getLeft());
    result += ",";
    visit_children(node.getRight());
    result += getNewlineAndTabString() + ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(CallIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Call(";
    visit_children(node.getTarget());
    if (node.getArgs().size() > 0) result += ",";
    for (auto &arg : node.getArgs()) {
        visit_children(arg);
        if (arg != node.getArgs().back()) result += ",";
    }
    result += getNewlineAndTabString() + ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(ConstIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Const(" + std::to_string(node.getValue()) + ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(ESeqIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() +"new ESeq(";
    visit_children(node.getStmt());
    result += ",";
    visit_children(node.getExpr());
    result += getNewlineAndTabString() + ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(MemIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Mem(";
    visit_children(node);
    result += getNewlineAndTabString() + ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(NameIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Name(\"" + getFunctionName(node.getName()) + "\")";
    num_tabs -=1;
}

void IRJavaConverter::operator()(TempIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Temp(\"" + node.getName() + "\")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(CJumpIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new CJump(";
    visit_children(node.getCondition());
    result += ", \"" + node.trueLabel() + "\", \"" + node.falseLabel() + "\")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(ExpIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Exp(";
    visit_children(node.getExpr());
    result += getNewlineAndTabString() + ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(JumpIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Jump(";
    visit_children(node.getTarget());
    result += getNewlineAndTabString() + ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(LabelIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Label(\"" + node.getName() + "\")";
    num_tabs -=1;
}

void IRJavaConverter::operator()(MoveIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Move(";
    visit_children(node.getTarget());
    result += ",";
    visit_children(node.getSource());
    result += getNewlineAndTabString() + ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(ReturnIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Return(";
    visit_children(node);
    result += ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(SeqIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() + "new Seq(";
    for (auto &stmt : node.getStmts()) {
        visit_children(stmt);

        if (stmt != node.getStmts().back()) {
            result += ",";
        }
    }
    result += getNewlineAndTabString() + ")";
    num_tabs -= 1;
}

std::string IRJavaConverter::getTabString() {
    std::string result = "";
    for (int i = 0; i < num_tabs; i++) {
        result += "\t";
    }

    return result;
}

std::string IRJavaConverter::getNewlineAndTabString() {
    return "\n" + getTabString();
}