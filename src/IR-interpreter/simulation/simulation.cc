#include "simulation.h"
#include <random>
#include <iostream>
#include <variant>
#include <cstdint> 
#include "utillities/overload.h"

std::string ABSTRACT_ARG_PREFIX = "_ARG";
std::string ABSTRACT_RET_PREFIX = "_RET";
int WORD_SIZE = 4;


Simulator::ExecutionFrame::ExecutionFrame(int ip, Simulator& parent) : ip(ip), parent(parent) {
    ret = rand();
}

int Simulator::ExecutionFrame::get(std::string tempName) {
    if (regs.find(tempName) == regs.end()) {
        put(tempName, rand());
    }

    return regs[tempName];
}

void Simulator::ExecutionFrame::put(std::string tempName, int value) {
    regs[tempName] = value;
}

bool Simulator::ExecutionFrame::advance() {
    if (debugLevel > 1) {
        // TODO: evaluate getCurrentNode() 
        std::cout << "Evaluating " << ip << std::endl;
    }

    int backupIp = ip;
    parent.leave(this); 

    if (ip == -1) return false;
    if (ip != backupIp) return true;

    ip++;
    return true;
}

void Simulator::ExecutionFrame::setIP(int ip) {
    this->ip = ip;

    if (debugLevel > 1) {
        if (ip == -1) {
            std::cout << "Returning" << std::endl;
        } else {
            std::cout << "Jumping to " << ip << std::endl; // TODO: evaluate getCurrentNode()
        }
    }
}

IR_PTR Simulator::ExecutionFrame::getCurrentNode() {
    if (parent.indexToNode.find(ip) == parent.indexToNode.end()) {
        THROW_SimulatorError("No next instruction. Forgot RETURN?");
    }

    return parent.indexToNode[ip];
}

Simulator::Simulator(IR *compUnit, int heapSizeMax) : heapSizeMax(heapSizeMax), exprStack(debugLevel) {
    if (std::holds_alternative<CompUnitIR>(*compUnit)) {
        this->compUnit = &std::get<CompUnitIR>(*compUnit);
    } else {
        THROW_SimulatorError("Expected CompUnitIR when creating Simulator");
    }

    libraryFunctions.insert("__malloc");
    libraryFunctions.insert("__debexit");
    libraryFunctions.insert("__exception");
    libraryFunctions.insert("NATIVEjava.io.OutputStream.nativeWrite");

    MapsBuilder mapBuilder;
    mapBuilder.visit(*compUnit);
    indexToNode = mapBuilder.getIndexToNode();
    nameToIndex = mapBuilder.getNameToIndex();
};


int Simulator::malloc(int size) {
    if (size < 0) THROW_SimulatorError("Invalid malloc size");

    if (size % WORD_SIZE != 0) {
        THROW_SimulatorError("Can only allocate in chunks of " + std::to_string(WORD_SIZE) + " bytes");
    }

    int retval = memory.size();
    if (retval + size > heapSizeMax) THROW_SimulatorError("Out of memory in the heap");

    for (int i = 0; i < size; i++) {
        memory.push_back(rand());
    }

    return retval;
}

int Simulator::calloc(int size) {
    int retval = malloc(size);
    for (int i = retval; i < retval + size; i++) {
        memory[i] = 0;
    }

    return retval;
}

int Simulator::read(int addr) {
    int i = getMemoryIndex(addr);
    if (i >= memory.size()) THROW_SimulatorError("Attempting to read past end of heap");

    return memory[i];
}

void Simulator::store(int addr, int value) {
    int i = getMemoryIndex(addr);
    if (i >= memory.size()) THROW_SimulatorError("Attempting to store past end of heap");
    
    memory[i] = value;
}

int Simulator::findLabel(std::string label) {
    return 1;
}

int Simulator::call(std::string name, std::vector<int> args) {
    auto frame = ExecutionFrame(-1, *this);
    return this->call(frame, name, args);
}

int Simulator::call(ExecutionFrame& parent, std::string name, std::vector<int> args) {
    int return_value;

    if (libraryFunctions.find(name) != libraryFunctions.end()) {
        return_value = libraryCall(name, args);
    } else {
        FuncDeclIR* func = compUnit->getFunc(name);
        if (func == nullptr) {
            THROW_SimulatorError("Function " + name + " not found");
        }

        int ip = findLabel(name);
        auto frame = std::make_unique<ExecutionFrame>(ip, *this);

        for (int i = 0; i < args.size(); i++) {
            frame->put(ABSTRACT_ARG_PREFIX + std::to_string(i), args[i]);
        }

        while (frame->advance());

        return_value = frame->ret;
    }

    return return_value;
}

int Simulator::getMemoryIndex(int addr) {
    if (addr % WORD_SIZE != 0) {
        THROW_SimulatorError("Unaligned memory access: " + std::to_string(addr) + " is not a multiple of " + std::to_string(WORD_SIZE));
    }

    return addr / WORD_SIZE;
}

int Simulator::libraryCall(std::string name, std::vector<int> args) {
    int return_value;

    if (name == "NATIVEjava.io.OutputStream.nativeWrite") {
        std::cout.put(static_cast<char>(static_cast<std::int8_t>(args[0]))); 
        return_value = 0;
    } else if (name == "__malloc") {
        return_value = malloc(args[0]);
    } else if (name == "__debexit") {
        std::exit(args[0]);
    } else if (name == "__exception") {
        std::exit(13);
    } else {
        THROW_SimulatorError("Unsupported library function: " + name);
    }

    return return_value;
}

void Simulator::leave(ExecutionFrame *frame) {
    IR_PTR node = frame->getCurrentNode();

    std::visit(util::overload{
        [&](ConstIR &cons) {
            Simulator::exprStack.pushValue(cons.getValue());
        },
        [&](TempIR &temp) {
            std::string tempName = temp.getName();
            Simulator::exprStack.pushValue(frame->get(tempName));
        },
        [&](BinOpIR &binop) {
            int r = Simulator::exprStack.popValue();
            int l = Simulator::exprStack.popValue();

            int result;
            switch (binop.op) {
                case BinOpIR::OpType::ADD:
                    result = l + r;
                    break;
                case BinOpIR::OpType::SUB:
                    result = l - r;
                    break;
                case BinOpIR::OpType::MUL:
                    result = l * r;
                    break;
                case BinOpIR::OpType::DIV:
                    if (r == 0) THROW_SimulatorError("Division by zero");
                    result = l / r;
                    break;
                case BinOpIR::OpType::MOD:
                    if (r == 0) THROW_SimulatorError("Division by zero");
                    result = l % r;
                    break;
                case BinOpIR::OpType::AND:
                    result = l & r;
                    break;
                case BinOpIR::OpType::OR:
                    result = l | r;
                    break;
                case BinOpIR::OpType::XOR:
                    result = l ^ r;
                    break;
                case BinOpIR::OpType::LSHIFT:
                    result = l << r;
                    break;
                case BinOpIR::OpType::RSHIFT:
                    result = static_cast<unsigned int>(l) >> r;
                    break;
                case BinOpIR::OpType::ARSHIFT:
                    result = l >> r;
                    break;
                case BinOpIR::OpType::EQ:
                    result = l == r ? 1 : 0;
                    break;
                case BinOpIR::OpType::NEQ:
                    result = l != r ? 1 : 0;
                    break;
                case BinOpIR::OpType::LT:
                    result = l < r ? 1 : 0;
                    break;
                case BinOpIR::OpType::GT:
                    result = l > r ? 1 : 0;
                    break;
                case BinOpIR::OpType::LEQ:
                    result = l <= r ? 1 : 0;
                    break;
                case BinOpIR::OpType::GEQ:
                    result = l >= r ? 1 : 0;
                    break;
                default:
                    THROW_SimulatorError("Unsupported binary operation");
            }

            Simulator::exprStack.pushValue(result);
        },
        [&](MemIR &mem){
            int addr = Simulator::exprStack.popValue();
            Simulator::exprStack.pushAddr(read(addr), addr);
        },
        [&](CallIR &call){
            int argCount = call.getArgs().size();
            std::vector<int> args;

            for (int i = 0; i < argCount; i++) {
                args.push_back(Simulator::exprStack.popValue());
            }

            auto target = Simulator::exprStack.pop();
            std::string targetName;

            if (target.type == StackItem::Kind::NAME) {
                targetName = target.name;
            } else if (Simulator::indexToNode.find(target.addr) != Simulator::indexToNode.end()) {
                auto ir_ptr = Simulator::indexToNode[target.addr];
                if (std::holds_alternative<FuncDeclIR*>(ir_ptr)) {
                    targetName = std::get<FuncDeclIR*>(ir_ptr)->getName();
                } else {
                    THROW_SimulatorError("Call to a non-function instruction");
                }
            } else {
                THROW_SimulatorError("Invalid function call " + call.label() + " (target " + std::to_string(target.value) + " is unknown)!");
            }
            
            int return_value = Simulator::call(*frame, targetName, args);
            Simulator::exprStack.pushValue(return_value);
        },
        [&](NameIR &name) {
            std::string str = name.getName();

            if (Simulator::libraryFunctions.find(str) != Simulator::libraryFunctions.end()) {
                Simulator::exprStack.pushName(-1, str);
            } else {
                int addr = findLabel(str);
                Simulator::exprStack.pushName(addr, str);
            }
        },
        [&](MoveIR &move) {
            int r = Simulator::exprStack.popValue();
            auto stackItem = Simulator::exprStack.pop();

            switch (stackItem.type) {
                case StackItem::Kind::MEM:
                    if (Simulator::debugLevel > 0) {
                        std::cout << "mem[" << stackItem.addr << "] = " << r << std::endl;
                    }
                    store(stackItem.addr, r);
                    break;
                case StackItem::Kind::TEMP:
                    if (Simulator::debugLevel > 0) {
                        std::cout << "temp[" << stackItem.name << "] = " << r << std::endl;
                    }
                    frame->put(stackItem.temp, r);
                default:
                    THROW_SimulatorError("Invalid MODE!");
            }
        },
        [&](ExpIR &exp) {
            Simulator::exprStack.pop();
        },
        [&](JumpIR &jump) {
            frame->setIP(Simulator::exprStack.popValue());
        },
        [&](CJumpIR &cjump) {
            int top = Simulator::exprStack.popValue();
            std::string label;

            if (top == 0) {
                label = cjump.falseLabel();
            } else if (top == 1) {
                label = cjump.trueLabel();
            } else {
                THROW_SimulatorError("Invalid condition for CJump, expected 0/1 and got " + top);
            }

            if (label != "") frame->setIP(findLabel(label));
        },
        [&](ReturnIR &ret) {
            frame->ret = Simulator::exprStack.popValue();
            frame->setIP(-1);
        },
        [&](auto &node) {}
    }, node);
}
