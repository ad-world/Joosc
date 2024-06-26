#include "proto_to_joos.h"
#include "joosc_proto.pb.h"

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace joosc_fuzzer {

// Constants / Settings
#define NUM_FIELDS 100
#define NUM_METHODS 4
const std::string class_name = "Foo";


// Tab printing
int num_tabs = 0;
void printTabs(std::ostream &os) {
    for ( int t = 0; t < num_tabs; t++ ) { os << "\t"; }
}
void printLessTabs(std::ostream &os) {
    num_tabs--;
    printTabs(os);
}

// Forward decl
std::ostream &operator<<(std::ostream &os, const Const &x);
std::ostream &operator<<(std::ostream &os, const BinaryOp &x);
std::ostream &operator<<(std::ostream &os, const MethodInvocation &x);
std::ostream &operator<<(std::ostream &os, const StatementSeq &x);
std::ostream &operator<<(std::ostream &os, const BoolToBoolOp &x);
std::ostream &operator<<(std::ostream &os, const IntToBoolOp &x);

// Proto to joos
std::ostream &operator<<(std::ostream &os, const VarRef &x) {
    return os << class_name << ".t[" << (static_cast<uint32_t>(x.varnum()) % NUM_FIELDS) << "]";
}

std::ostream &operator<<(std::ostream &os, const FunctionRef &x) {
    return os << class_name << ".m" << (static_cast<uint32_t>(x.funcnum()) % NUM_METHODS);
}

std::ostream &operator<<(std::ostream &os, const Lvalue &x) {
    return os << x.varref();
}

std::ostream &operator<<(std::ostream &os, const Rvalue &x) {
    switch ( x.rvalue_oneof_case() ) {
        case Rvalue::kVarref:
            return os << x.varref();
        case Rvalue::kBinop:
            return os << x.binop();
        case Rvalue::kMethodInvoc:
            return os << x.method_invoc();
        case Rvalue::RVALUE_ONEOF_NOT_SET:
        default:
            return os << x.cons();
    }
}

std::ostream &operator<<(std::ostream &os, const Const &x) {
    return os << "(" << x.val() << ")";
}

std::ostream &operator<<(std::ostream &os, const BinaryOp &x) {
    os << "(" << x.left();
    switch ( x.op() ) {
        case BinaryOp::ADD: os << "+"; break;
        case BinaryOp::SUB: os << "-"; break;
        case BinaryOp::MUL: os << "*"; break;
        case BinaryOp::DIV: os << "/"; break;
        case BinaryOp::MOD: os << "%"; break;
    }
    return os << x.right() << ")";
}

std::ostream &operator<<(std::ostream &os, const BoolRvalue &x) {
    switch ( x.bool_binop_oneof_case() ) {
        case BoolRvalue::kIntop:
            return os << x.intop();
        case BoolRvalue::kBoolop:
            return os << x.boolop();
        case BoolRvalue::BOOL_BINOP_ONEOF_NOT_SET:
        default:
            return os << (x.cons() ? "(true)" : "(false)");
    }
}

std::ostream &operator<<(std::ostream &os, const BoolToBoolOp &x) {
    os << "(" << x.left();
    switch ( x.op() ) {
        case BoolToBoolOp::AND: os << "&&"; break;
        case BoolToBoolOp::OR: os << "||"; break;
        case BoolToBoolOp::EAND: os << "&"; break;
        case BoolToBoolOp::EOR: os << "|"; break;
        case BoolToBoolOp::EQ: os << "=="; break;
        case BoolToBoolOp::NEQ: os << "!="; break;
    }
    return os << x.right() << ")";
}

std::ostream &operator<<(std::ostream &os, const IntToBoolOp &x) {
    os << "(" << x.left();
    switch ( x.op() ) {
        case IntToBoolOp::LT: os << "<"; break;
        case IntToBoolOp::GT: os << ">"; break;
        case IntToBoolOp::LEQ: os << "<="; break;
        case IntToBoolOp::GEQ: os << ">="; break;
        case IntToBoolOp::EQ: os << "=="; break;
        case IntToBoolOp::NEQ: os << "!="; break;
    }
    return os << x.right() << ")";
}

std::ostream &operator<<(std::ostream &os, const MethodInvocation &x) {
    return os << x.funcref() << "()";
}

///////////////////////////////////////
//              Statements
///////////////////////////////////////
std::ostream &operator<<(std::ostream &os, const AssignmentStatement &x) {
    printTabs(os);
    return os << x.lvalue() << "=" << x.rvalue() << ";\n";
}

std::ostream &operator<<(std::ostream &os, const MethodInvocationStatement &x) {
    printTabs(os);
    return os << x.method_invocation() << ";\n";
}

// std::ostream &operator<<(std::ostream &os, const VarDecl &x) {
//     printTabs(os);
//     return os << "int " << x.lvalue() << "=" << x.rvalue() << ";\n";
// }

std::ostream &operator<<(std::ostream &os, const IfThen &x) {
    printTabs(os); os << "if (" << x.cond() << "){\n"; num_tabs++;
    os << x.if_body();
    printLessTabs(os); os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, const IfElse &x) {
    printTabs(os); os << "if (" << x.cond() << "){\n"; num_tabs++;
    os << x.if_body();
    printLessTabs(os); os << "} else { \n"; num_tabs++;
    os << x.else_body();
    printLessTabs(os); os << "}\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, const While &x) {
    printTabs(os); os << "while (" << x.cond() << "){\n"; num_tabs++;
    os << x.body();
    printLessTabs(os); os << "}\n";
    return (os);
}

std::ostream &operator<<(std::ostream &os, const ReturnStatement &x) {
    printTabs(os);
    return os << "return (" << x.rval() << ");\n";
}

std::ostream &operator<<(std::ostream &os, const Statement &x) {
    switch ( x.stmt_oneof_case() ) {
        case Statement::kIfthen:
            return os << x.ifthen();
        case Statement::kIfelse:
            return os << x.ifelse();
        case Statement::kWhileLoop:
            return os << x.while_loop();
        case Statement::kRetStmt:
            return os << x.ret_stmt();
        case Statement::kMethodCall:
            return os << x.method_call();
        case Statement::STMT_ONEOF_NOT_SET:
        default:
            return os << x.assignment();
    }
}
///////////////////////////////////////

std::ostream &operator<<(std::ostream &os, const StatementSeq &x) {
    for ( auto &stmt : x.statements() ) { os << stmt; }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Function &x) {
    return os << x.body() << x.ret_stmt();
}

std::ostream &operator<<(std::ostream &os, const StaticField &x) {
    printTabs(os);
    return os << x.lvalue() << "=" << x.cons() << ";\n";
}

std::ostream &operator<<(std::ostream &os, const Class &x) {
    os  << "public class " << class_name << " {\n"; num_tabs++;

    // Static fields
    printTabs(os);
    os << "public static int[] t = new int[" << NUM_FIELDS << "];\n";

    // Constructor
    printTabs(os);
    os << "public Foo() {\n"; num_tabs++;
    for ( auto field : x.fields() ) {
        // Initialize static fields
        os << field;
    }
    printLessTabs(os);
    os << "}\n";

    // Main function
    printTabs(os);
    os << "public static int test() {\n"; num_tabs++;
    os << x.main_body();
    printLessTabs(os);
    os << "}\n";

    // Static methods
    auto method_it = x.methods().begin();
    for ( int m = 0; m < NUM_METHODS; m++ ) {
        std::string method_name = "m" + std::to_string(m);
        printTabs(os);
        os << "public static int " << method_name << "() {\n"; num_tabs++;

        // Print body if exists
        if ( method_it != x.methods().end() ) {
            os << *method_it;
            method_it++;
        } else {
            printTabs(os);
            os << "return 1;\n"; // default return for functions
        }

        printLessTabs(os);
        os << "}\n";
    }

    // End of class
    printLessTabs(os);
    return os << "}\n";
}

// ----------------------------------

std::string ClassToString(const Class &input) {
    std::ostringstream os;
    os << input;
    return os.str();
}

std::string ProtoToJoos(const uint8_t *data, size_t size) {
    Class message;
    if ( !message.ParsePartialFromArray(data, size) ) {
        return "#error invalid proto\n";
    }
    return ClassToString(message);
}

} // namespace joosc_fuzzer
