#include "proto_to_joos.h"
#include "joosc_proto.pb.h"

#include <ostream>
#include <sstream>

namespace joosc_fuzzer {

// Forward decl
std::ostream &operator<<(std::ostream &os, const Const &x);
std::ostream &operator<<(std::ostream &os, const BinaryOp &x);
std::ostream &operator<<(std::ostream &os, const MethodInvocation &x);
std::ostream &operator<<(std::ostream &os, const StatementSeq &x);

// Proto to joos
std::ostream &operator<<(std::ostream &os, const VarRef &x) {
    return os << "t" << (static_cast<uint32_t>(x.varnum()) % 25);
}

std::ostream &operator<<(std::ostream &os, const FunctionRef &x) {
    return os << "m" << (static_cast<uint32_t>(x.funcnum()) % 4);
}

std::ostream &operator<<(std::ostream &os, const Lvalue &x) {
    return os << x.varref();
}

std::ostream &operator<<(std::ostream &os, const Rvalue &x) {
    if ( x.has_varref() ) return os << x.varref();
    if ( x.has_cons() ) return os << x.cons();
    if ( x.has_binop() ) return os << x.binop();
    if ( x.has_method_invoc() ) return os << x.method_invoc();
    return os << "1";
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
        case BinaryOp::AND: os << "&&"; break;
        case BinaryOp::OR: os << "||"; break;
        case BinaryOp::EAND: os << "&"; break;
        case BinaryOp::EOR: os << "|"; break;
        case BinaryOp::EQ: os << "=="; break;
        case BinaryOp::NEQ: os << "!="; break;
        case BinaryOp::LT: os << "<"; break;
        case BinaryOp::GT: os << ">"; break;
        case BinaryOp::LEQ: os << "<="; break;
        case BinaryOp::GEQ: os << ">="; break;
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
    return os << x.lvalue() << "=" << x.rvalue() << ";\n";
}

std::ostream &operator<<(std::ostream &os, const MethodInvocationStatement &x) {
    return os << x.method_invocation() << ";\n";
}

std::ostream &operator<<(std::ostream &os, const VarDecl &x) {
    return os << "int " << x.lvalue() << "=" << x.rvalue() << ";\n";
}

std::ostream &operator<<(std::ostream &os, const IfThen &x) {
    return os << "if (" << x.cond() << "){\n"
              << x.if_body() << "}\n";
}

std::ostream &operator<<(std::ostream &os, const IfElse &x) {
    return os << "if (" << x.cond() << "){\n"
              << x.if_body() << "} else { \n"
              << x.else_body() << "}\n";
}

std::ostream &operator<<(std::ostream &os, const While &x) {
    return os << "while (" << x.cond() << "){\n"
              << x.body() << "}\n";
}

std::ostream &operator<<(std::ostream &os, const Statement &x) {
    if ( x.has_assignment() ) return os << x.assignment();
    if ( x.has_ifthen() ) return os << x.ifthen();
    if ( x.has_ifelse() ) return os << x.ifelse();
    if ( x.has_while_loop() ) return os << x.while_loop();
    if ( x.has_method_call() ) return os << x.method_call();
    if ( x.has_declaration() ) return os << x.declaration();
    return os << ";\n";
}
///////////////////////////////////////

std::ostream &operator<<(std::ostream &os, const StatementSeq &x) {
    for ( auto &stmt : x.statements() ) { os << stmt; }
    return os;
}

std::ostream &operator<<(std::ostream &os, const StaticField &x) {
    return os << "public static int " << x.lvalue() << "=" << x.cons() << ";\n";
}

std::ostream &operator<<(std::ostream &os, const Class &x) {
    os  << "public class Foo {\n"
            << "public Foo() {}\n"
            << "public static void test() {\n"
                << x.main_body()
            << "}\n";
    for ( auto &field : x.fields() ) {
        os << field;
    }
    if ( x.has_method0_body() ) {
        os  << "public static void m0() {\n"
                << x.method0_body()
            << "}\n";
    }
    if ( x.has_method1_body() ) {
        os  << "public static void m1() {\n"
                << x.method1_body()
            << "}\n";
    }
    if ( x.has_method2_body() ) {
        os  << "public static void m2() {\n"
                << x.method2_body()
            << "}\n";
    }
    if ( x.has_method3_body() ) {
        os  << "public static void m3() {\n"
                << x.method3_body()
            << "}\n";
    }
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
