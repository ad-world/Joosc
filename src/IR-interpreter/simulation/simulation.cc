#include "simulation.h"
#include <random>
#include <iostream>
#include <variant>

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
    // TODO: uncomment the leave
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

IR* Simulator::ExecutionFrame::getCurrentNode() {
    if (parent.indexToNode.find(ip) == parent.indexToNode.end()) {
        throw new Trap("No next instruction. Forgot RETURN?");
    }

    return parent.indexToNode[ip];
}

