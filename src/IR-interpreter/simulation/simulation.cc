#include "simulation.h"
#include <random>
#include <iostream>
#include <string>
#include <variant>
#include <cstdint> 
#include "exceptions/exceptions.h"
#include "utillities/overload.h"

#include "IR/code-gen-constants.h"

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
        std::cout << "\033[1;31mEvaluating " << parent.getNodeType(getCurrentNode()) << " with ip: " << ip << "\033[0m" << std::endl;
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

    exprStack = ExprStack(debugLevel);

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
    if (i >= memory.size()) throw ExitSysCall(13);

    return memory[i];
}

void Simulator::store(int addr, int value) {
    int i = getMemoryIndex(addr);
    if (i >= memory.size()) throw ExitSysCall(13);
    
    memory[i] = value;
}

int Simulator::findLabel(std::string label) {
    if (nameToIndex.find(label) == nameToIndex.end()) {
        THROW_SimulatorError("Could not find label '" + label + "'!");
    }
    return nameToIndex[label];
}

std::string Simulator::getNodeType(IR_PTR node) {
    return std::visit([&](auto *x) { return x->label(); }, node);
}

int Simulator::call(std::string name, std::vector<int> args) {
    try {
        auto frame = ExecutionFrame(-1, *this);
        return call(frame, name, args);
    } catch (ExitSysCall &e) {
        return e.rc;
    }
}

int Simulator::call(ExecutionFrame& parent, std::string name, std::vector<int> args) {
#ifdef LIBFUZZER
    depth++;
    if ( depth > max_depth ) {
        THROW_SoftSimulatorError("Max function call depth exceeded. (Most likely recursion)");
    }
#endif
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
            if (debugLevel > 1) {
                std::cout << "Adding " << CGConstants::ABSTRACT_ARG_PREFIX + std::to_string(i) << " = " << args[i] << " to args" << std::endl;
            }
            frame->put(CGConstants::ABSTRACT_ARG_PREFIX + std::to_string(i), args[i]);
        }

        while (frame->advance());

        return_value = frame->ret;

        if (debugLevel > 1)
            std::cout << "Frame return value is " << return_value << std::endl;
    }

    parent.put(CGConstants::ABSTRACT_RET, return_value);
    if (debugLevel > 1)
        std::cout << "Calling " << name << " returned " << return_value << std::endl;
 
#ifdef LIBFUZZER
    depth--;
#endif

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
        //std::exit(args[0]);
        // return_value = args[0];
        throw ExitSysCall(args[0]);
    } else if (name == "__exception") {
        //std::exit(13);
        // return_value = 13;
        throw ExitSysCall(13);
    } else {
        THROW_SimulatorError("Unsupported library function: " + name);
    }

    return return_value;
}

void Simulator::leave(ExecutionFrame *frame) {
    IR_PTR node = frame->getCurrentNode();

    std::visit(util::overload{
        [&](ConstIR *cons) {
            Simulator::exprStack.pushValue(cons->getValue());
        },
        [&](TempIR *temp) {
            std::string tempName = temp->getName();
            Simulator::exprStack.pushTemp(frame->get(tempName), tempName);
        },
        [&](BinOpIR *binop) {
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
                    if (r == 0) throw ExitSysCall(13);
                    //THROW_SoftSimulatorError("Division by zero");
                    result = l / r;
                    break;
                case BinOpIR::OpType::MOD:
                    if (r == 0) throw ExitSysCall(13);
                    //THROW_SoftSimulatorError("Division by zero");
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
            int addr = Simulator::exprStack.popValue();
            Simulator::exprStack.pushAddr(read(addr), addr);
        },
        [&](CallIR *call){
            int argCount = call->getArgs().size();
            std::vector<int> args;

            for (int i = 0; i < argCount; i++) {
                args.push_back(Simulator::exprStack.popValue());
            }

            // Reverse args back to original order
            for ( int i = 0; i < argCount / 2; i++ ) {
                std::swap(args[i], args[argCount - i - 1]);
            }

            auto target = Simulator::exprStack.pop();
            std::string targetName;

            if (target.type == StackItem::Kind::NAME) {
                targetName = target.name;
            } else if (Simulator::indexToNode.find(target.value) == Simulator::indexToNode.end()) {
                auto ir_ptr = Simulator::indexToNode[target.value];
                if (std::holds_alternative<FuncDeclIR*>(ir_ptr)) {
                    targetName = std::get<FuncDeclIR*>(ir_ptr)->getName();
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
            std::string str = name->getName();

            if (Simulator::libraryFunctions.find(str) != Simulator::libraryFunctions.end()) {
                Simulator::exprStack.pushName(-1, str);
            } else {
                int label = findLabel(str);
                Simulator::exprStack.pushName(label, str);
            }
        },
        [&](MoveIR *move) {
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
            Simulator::exprStack.pop();
        },
        [&](JumpIR *jump) {
            frame->setIP(Simulator::exprStack.popValue());
        },
        [&](CJumpIR *cjump) {
            int top = Simulator::exprStack.popValue();
            std::string label;

            if (top == 0) {
                label = cjump->falseLabel();
            } else if (top == 1) {
                label = cjump->trueLabel();
            } else {
                THROW_SimulatorError("Invalid condition for CJump, expected 0/1 and got " + std::to_string(top));
            }

            if (label != "") frame->setIP(findLabel(label));
        },
        [&](ReturnIR *ret) {
            frame->ret = Simulator::exprStack.popValue();
            if (Simulator::debugLevel > 0) 
                std::cout << "Encountered return, returning " << frame->ret << std::endl;
            frame->setIP(-1);
        },
        [&](auto &node) {
            if (Simulator::debugLevel > 0) 
                std::cout << "Leaving: " << node->label() << std::endl;
        }
    }, node);
}

void Simulator::setDebugLevel(int level) {
    Simulator::debugLevel = level;
}

