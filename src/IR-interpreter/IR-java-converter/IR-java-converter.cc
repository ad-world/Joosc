#include "IR-java-converter.h"

IRJavaConverter::IRJavaConverter(std::string class_name): class_name(class_name) {
    num_tabs = 0;
    result = "";
};

void IRJavaConverter::appendToResult(std::string s) {
    result += getNewlineAndTabString() + s;
}

void IRJavaConverter::operator()(CompUnitIR &node) {
    result += "package joosc.ir.interpret;\nimport joosc.ir.ast.*;\nimport joosc.ir.visit.CheckCanonicalIRVisitor;\n\n";
    result += "public class " + class_name + " {";
    num_tabs += 1;
    appendToResult("public static void main(String[] args) {");
    visit_children(node);

    num_tabs += 1;

    appendToResult("CompUnit compUnit = new CompUnit(\"test\");");
    for (auto &func : functions) {
        appendToResult("compUnit.appendFunc(" + func + ");");
    }

    appendToResult("Simulator sim = new Simulator(compUnit);");
    appendToResult("long result = sim.call(\"test\");");
    // appendToResult("System.out.println(\"Java Interpretation of IR results to: \" + result);");
    appendToResult("CheckCanonicalIRVisitor cv = new CheckCanonicalIRVisitor();");
    appendToResult("System.out.print(\"Correct canonical implementation: \");");
    appendToResult("System.out.println(cv.visit(compUnit));");

    appendToResult("System.exit((int)result);");
    num_tabs -= 1;
    appendToResult("}");
    num_tabs -= 1;
    appendToResult("}");
}

void IRJavaConverter::operator()(FuncDeclIR &node) {

    std::string function_name = node.getName() + "Func";
    functions.push_back(function_name); 

    num_tabs += 1;
    appendToResult("Stmt " + node.getName() + "Body = ");
    visit_children(node);
    result += ";";
    appendToResult("FuncDecl " + function_name + " = new FuncDecl(\"" + node.getName() + "\"," + std::to_string(node.getNumParams()) + "," + node.getName() + "Body);\n");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(BinOpIR &node) {
    num_tabs += 1;
    appendToResult("new BinOp(BinOp.OpType." + node.label() + ",");
    visit_children(node.getLeft());
    result += ",";
    visit_children(node.getRight());
    appendToResult(")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(CallIR &node) {
    num_tabs += 1;
    appendToResult("new Call(");
    visit_children(node.getTarget());
    if (node.getArgs().size() > 0) result += ",";
    for (auto &arg : node.getArgs()) {
        visit_children(arg);
        if (arg != node.getArgs().back()) result += ",";
    }
    appendToResult(")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(ConstIR &node) {
    num_tabs += 1;
    appendToResult("new Const(" + std::to_string(node.getValue()) + ")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(ESeqIR &node) {
    num_tabs += 1;
    result += getNewlineAndTabString() +"new ESeq(";
    visit_children(node.getStmt());
    result += ",";
    visit_children(node.getExpr());
    appendToResult(")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(MemIR &node) {
    num_tabs += 1;
    appendToResult("new Mem(");
    visit_children(node);
    appendToResult(")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(NameIR &node) {
    num_tabs += 1;
    appendToResult("new Name(\"" + node.getName() + "\")");
    num_tabs -=1;
}

void IRJavaConverter::operator()(TempIR &node) {
    num_tabs += 1;
    appendToResult("new Temp(\"" + node.getName() + "\")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(CJumpIR &node) {
    num_tabs += 1;
    appendToResult("new CJump(");
    visit_children(node.getCondition());
    result += ", \"" + node.trueLabel() + "\", \"" + node.falseLabel() + "\")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(ExpIR &node) {
    num_tabs += 1;
    appendToResult("new Exp(");
    visit_children(node.getExpr());
    appendToResult(")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(JumpIR &node) {
    num_tabs += 1;
    appendToResult("new Jump(");
    visit_children(node.getTarget());
    appendToResult(")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(LabelIR &node) {
    num_tabs += 1;
    appendToResult("new Label(\"" + node.getName() + "\")");
    num_tabs -=1;
}

void IRJavaConverter::operator()(MoveIR &node) {
    num_tabs += 1;
    appendToResult("new Move(");
    visit_children(node.getTarget());
    result += ",";
    visit_children(node.getSource());
    appendToResult(")");
    num_tabs -= 1;
}

void IRJavaConverter::operator()(ReturnIR &node) {
    num_tabs += 1;
    appendToResult("new Return(");
    visit_children(node);
    result += ")";
    num_tabs -= 1;
}

void IRJavaConverter::operator()(SeqIR &node) {
    num_tabs += 1;
    appendToResult("new Seq(");
    for (auto &stmt : node.getStmts()) {
        visit_children(stmt);

        if (stmt != node.getStmts().back()) {
            result += ",";
        }
    }
    appendToResult(")");
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