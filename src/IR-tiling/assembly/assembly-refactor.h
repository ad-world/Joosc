#pragma once

#include <string>
#include "assembly-common.h"

// File that contains the classes for each used x86 assembly instruction.
//
// Each added instruction should be added to the variant in assembly-instruction.h.
// Each added instruction should implement toString(), and define which operands are read/written to (can be both)

/* Assorted instructions */

struct Mov : public AssemblyCommon {
    Operand dest, source;

    Mov(Operand dest, Operand src) : dest{dest}, source{src} {
        useWriteOperands(this->dest);
        useReadOperands(this->source);
    }

    std::string toString() {
        return "mov " + dest.toString() + " " + source.toString();
    }
};

struct Jump : public AssemblyCommon {
    Operand target;

    Jump(Operand target) : target{target} {
        useReadOperands(this->target);
    }

    std::string toString() {
        return "jmp " + target.toString();
    }
};

struct Je : public AssemblyCommon {
    Operand target;

    Je(Operand target) : target{target} {
        useReadOperands(this->target);
    }

    std::string toString() {
        return "je " + target.toString();
    }
};

struct JumpIfNZ : public AssemblyCommon {
    Operand target;

    JumpIfNZ(Operand target) : target{target} {
        useReadOperands(this->target);
    }

    std::string toString() {
        return "jnz " + target.toString();
    }
};

struct Lea : public AssemblyCommon {
    Operand dest, source;

    Lea(Operand dest, Operand src) : dest{dest}, source{src} {
        useWriteOperands(this->dest);
        useReadOperands(this->source);

        if (!std::get_if<EffectiveAddress>(&src)) {
            THROW_CompilerError("Lea source must be effective address!");
        }
    }

    std::string toString() {
        return "lea " + dest.toString() + ", " + source.toString();
    }
};

struct Add : public AssemblyCommon {
    Operand arg1, arg2;

    Add(Operand arg1, Operand arg2) : arg1{arg1}, arg2{arg2} {
        useWriteOperands(this->arg1);
        useReadOperands(this->arg1, this->arg2);
    }

    std::string toString() {
        return "add " + arg1.toString() + ", " + arg2.toString();
    }
};

struct Sub : public AssemblyCommon {
    Operand arg1, arg2;

    Sub(Operand arg1, Operand arg2) : arg1{arg1}, arg2{arg2} {
        useWriteOperands(this->arg1);
        useReadOperands(this->arg1, this->arg2);
    }

    std::string toString() {
        return "sub " + arg1.toString() + ", " + arg2.toString();
    }
};

struct Xor : public AssemblyCommon {
    Operand arg1, arg2;

    Xor(Operand arg1, Operand arg2) : arg1{arg1}, arg2{arg2} {
        useWriteOperands(this->arg1);
        useReadOperands(this->arg1, this->arg2);
    }

    std::string toString() {
        return "xor " + arg1.toString() + ", " + arg2.toString();
    }
};

struct And : public AssemblyCommon {
    Operand arg1, arg2;

    And(Operand arg1, Operand arg2) : arg1{arg1}, arg2{arg2} {
        useWriteOperands(this->arg1);
        useReadOperands(this->arg1, this->arg2);
    }

    std::string toString() {
        return "and " + arg1.toString() + ", " + arg2.toString();
    }
};

struct Or : public AssemblyCommon {
    Operand arg1, arg2;

    Or(Operand arg1, Operand arg2) : arg1{arg1}, arg2{arg2} {
        useWriteOperands(this->arg1);
        useReadOperands(this->arg1, this->arg2);
    }

    std::string toString() {
        return "or " + arg1.toString() + ", " + arg2.toString();
    }
};

struct MovZX : public AssemblyCommon {
    Operand arg1, arg2;

    MovZX(Operand arg1, Operand arg2) : arg1{arg1}, arg2{arg2} {
        useWriteOperands(this->arg1);
        useReadOperands(this->arg2);
    }

    std::string toString() {
        return "movzx " + arg1.toString() + ", " + arg2.toString();
    }
};

struct Cmp : public AssemblyCommon {
    Operand arg1, arg2;

    Cmp(Operand arg1, Operand arg2) : arg1{arg1}, arg2{arg2} {
        useReadOperands(this->arg1, this->arg2);
    }

    std::string toString() {
        return "cmp " + arg1.toString() + ", " + arg2.toString();
    }
};

struct Test : public AssemblyCommon {
    Operand arg1, arg2;

    Test(Operand arg1, Operand arg2) : arg1{arg1}, arg2{arg2} {
        useReadOperands(this->arg1, this->arg2);
    }

    std::string toString() {
        return "test " + arg1.toString() + ", " + arg2.toString();
    }
};

struct Push : public AssemblyCommon {
    Operand arg;

    Push(Operand arg) : arg{arg} {
        useReadOperands(this->arg);
    }

    std::string toString() {
        return "push " + arg.toString();
    }
};

struct Pop : public AssemblyCommon {
    Operand arg;

    Pop(Operand arg) : arg{arg} {
        useWriteOperands(this->arg);
    }

    std::string toString() {
        return "pop " + arg.toString();
    }
};

/* Instructions without operands */

struct NoOperandInstruction : public AssemblyCommon {
    std::string static_instruction;

    NoOperandInstruction(std::string static_instruction) : static_instruction{static_instruction} {}

    std::string toString() {
        return static_instruction;
    }
};

struct Cdq : public NoOperandInstruction {
    Cdq() : NoOperandInstruction{"cdq"} {}
};

struct Ret : public NoOperandInstruction {
    Ret(unsigned int bytes = 0) 
        : NoOperandInstruction{bytes > 0 ? "ret " + std::to_string(bytes) : "ret"} 
    {}
};

struct Call : public NoOperandInstruction {
    Call(std::string static_label) 
        : NoOperandInstruction{"call " + static_label} 
    {}
};

struct SysCall : public NoOperandInstruction {
    SysCall() 
        : NoOperandInstruction{"int 0x80"} 
    {}
};

/* SetX instructions which set destination to 1 or 0 based on flags from Cmp */

struct BoolSetInstruction : public AssemblyCommon {
    std::string instruction_name;
    Operand dest;

    BoolSetInstruction(std::string name, Operand dest) 
        : instruction_name{name}, dest{dest} 
    {
        useWriteOperands(this->dest);
    }

    std::string toString() {
        return instruction_name + " " + dest.toString();
    }
};

struct SetZ : public BoolSetInstruction {
    SetZ(Operand dest) : BoolSetInstruction{"setz", dest} {}
};

struct SetNZ : public BoolSetInstruction {
    SetNZ(Operand dest) : BoolSetInstruction{"setnz", dest} {}
};

struct SetL : public BoolSetInstruction {
    SetL(Operand dest) : BoolSetInstruction{"setl", dest} {}
};

struct SetG : public BoolSetInstruction {
    SetG(Operand dest) : BoolSetInstruction{"setg", dest} {}
};

struct SetLE : public BoolSetInstruction {
    SetLE(Operand dest) : BoolSetInstruction{"setle", dest} {}
};

struct SetGE : public BoolSetInstruction {
    SetGE(Operand dest) : BoolSetInstruction{"setge", dest} {}
};

/* IMul/IDiv */

struct IMul : public AssemblyCommon {
    Operand multiplicand;

    IMul(Operand multiplicand) : multiplicand{multiplicand} {
        useReadOperands(this->multiplicand);
    }

    std::string toString() {
        return "imul " + multiplicand.toString();
    }
};

struct IDiv : public AssemblyCommon {
    Operand divisor;

    IDiv(Operand divisor) : divisor{divisor} {
        useReadOperands(this->divisor);
    }

    std::string toString() {
        return "idiv " + divisor.toString();
    }
};
