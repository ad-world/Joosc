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
    if (parent.debugLevel > 1) {
        std::cout << "Evaluating " << parent.getNodeType(getCurrentNode()) << " with ip: " << ip << std::endl;
    }

    int backupIp = ip;

    if (ip >= parent.indexToNode.size()) return false;

    parent.leave(this); 

    if (ip == -1) return false;
    if (ip != backupIp) return true;

    ip++;
    return true;
}

void Simulator::ExecutionFrame::setIP(int ip) {
    this->ip = ip;

    if (parent.debugLevel > 1) {
        if (ip == -1) {
            std::cout << "Returning" << std::endl;
        } else {
            std::cout << "Jumping to " << parent.getNodeType(getCurrentNode()) << " with ip: " << ip << std::endl; 
        }
    }
}

IR_PTR Simulator::ExecutionFrame::getCurrentNode() {
    if (parent.indexToNode.find(ip) == parent.indexToNode.end()) {
        THROW_SimulatorError("Looking for ip and could not find it.");
    }

    return parent.indexToNode[ip];
}

Simulator::Simulator(IR *compUnit, int heapSizeMax) : heapSizeMax(heapSizeMax) {
    if (std::holds_alternative<CompUnitIR>(*compUnit)) {
        this->compUnit = &std::get<CompUnitIR>(*compUnit);
    } else {
        THROW_SimulatorError("Expected CompUnitIR when creating Simulator");
    }

    exprStack = ExprStack(2);
    setDebugLevel(2);

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
    if (nameToIndex.find(label) == nameToIndex.end()) {
        int last_dot = label.find_last_of('.');
        if (last_dot != std::string::npos) {
            std::string short_label = label.substr(last_dot + 1);
            if (nameToIndex.find(short_label) == nameToIndex.end()) {
                THROW_SimulatorError("Could not find label '" + label + "'!");
            } else {
                return nameToIndex[short_label];
            }
        } else {
            THROW_SimulatorError("Could not find label '" + label + "'!");

        }
    }
    return nameToIndex[label];
}

std::string Simulator::getNodeType(IR_PTR node) {
    return std::visit([&](auto *x) { return x->label(); }, node);
}

int Simulator::call(std::string name, std::vector<int> args) {
    auto frame = ExecutionFrame(-1, *this);
    return call(frame, name, args);
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
            std::cout << "Adding " << ABSTRACT_ARG_PREFIX + std::to_string(i) << " = " << args[i] << " to args" << std::endl;
            frame->put(ABSTRACT_ARG_PREFIX + std::to_string(i), args[i]);
        }

        while (frame->advance());

        return_value = frame->ret;

        std::cout << "Frame return value is " << return_value << std::endl;
    }

    parent.put(ABSTRACT_RET_PREFIX, return_value);
    std::cout << "Calling " << name << " returned " << return_value << std::endl;
 
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
        [&](ConstIR *cons) {
            // std::cout << "CONST" << std::endl;
            Simulator::exprStack.pushValue(cons->getValue());
        },
        [&](TempIR *temp) {
            // std::cout << "TEMP" << std::endl;

            std::string tempName = temp->getName();
            Simulator::exprStack.pushTemp(frame->get(tempName), tempName);
        },
        [&](BinOpIR *binop) {
            // std::cout << "BINOP" << std::endl;

            int r = Simulator::exprStack.popValue();
            int l = Simulator::exprStack.popValue();

            int result;
            switch (binop->op) {
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
        [&](MemIR *mem){
            // std::cout << "MEM" << std::endl;

            int addr = Simulator::exprStack.popValue();
            Simulator::exprStack.pushAddr(read(addr), addr);
        },
        [&](CallIR *call){
            // std::cout << "CALL" << std::endl;

            int argCount = call->getArgs().size();
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
                    int last_dot = targetName.find_last_of('.');

                    if (last_dot != std::string::npos) {
                        targetName = targetName.substr(last_dot + 1);
                    }

                } else {
                    THROW_SimulatorError("Call to a non-function instruction");
                }
            } else {
                std::string message = "Invalid function call " + call->label() + " (target " + std::to_string(target.value) + " is unknown)!";
                THROW_SimulatorError(message);
            }
            
            int return_value = Simulator::call(*frame, targetName, args);
            Simulator::exprStack.pushValue(return_value);
        },
        [&](NameIR *name) {
            // std::cout << "NAME" << std::endl;

            std::string str = name->getName();

            if (Simulator::libraryFunctions.find(str) != Simulator::libraryFunctions.end()) {
                Simulator::exprStack.pushName(-1, str);
            } else {
                int addr = findLabel(str);
                Simulator::exprStack.pushName(addr, str);
            }
        },
        [&](MoveIR *move) {
            // std::cout << "MOVE" << std::endl;

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
                        std::cout << "temp[" << stackItem.temp << "] = " << r << std::endl;
                    }
                    frame->put(stackItem.temp, r);
                    break;
                default:
                    THROW_SimulatorError("Invalid MOVE - " + stackItem.getKindString());
            }
        },
        [&](ExpIR *exp) {
            // std::cout << "EXPR" << std::endl;

            Simulator::exprStack.pop();
        },
        [&](JumpIR *jump) {
            // std::cout << "JUMP" << std::endl;

            frame->setIP(Simulator::exprStack.popValue());
        },
        [&](CJumpIR *cjump) {
            // std::cout << "CJUMP" << std::endl;

            int top = Simulator::exprStack.popValue();
            std::string label;

            if (top == 0) {
                label = cjump->falseLabel();
            } else if (top == 1) {
                label = cjump->trueLabel();
            } else {
                THROW_SimulatorError("Invalid condition for CJump, expected 0/1 and got " + top);
            }

            if (label != "") frame->setIP(findLabel(label));
        },
        [&](ReturnIR *ret) {
            frame->ret = Simulator::exprStack.popValue();
            std::cout << "Encountered return, returning " << frame->ret << std::endl;
            frame->setIP(-1);
        },
        [&](auto &node) {
            std::cout << "Leaving: " << node->label() << std::endl;
        }
    }, node);
}

void Simulator::setDebugLevel(int level) {
    Simulator::debugLevel = level;
}

