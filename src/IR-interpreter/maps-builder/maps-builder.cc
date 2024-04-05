#include "maps-builder.h"
#include "IR-interpreter/util/errors.h"
#include "utillities/overload.h"
#include <iostream>

MapsBuilder::MapsBuilder() : index(0) {}

void MapsBuilder::addNameToCurrentIndex(std::string name) {
    if (nameToIndex.find(name) != nameToIndex.end()) {
        throw new InternalCompilerError("Error - encountered duplicate name " + name + " in the IR tree -- go fix the generator.");
    }
    nameToIndex[name] = index;
}

std::unordered_map<std::string, int> &MapsBuilder::getNameToIndex() {
    return nameToIndex;
}

std::unordered_map<int, IR_PTR> &MapsBuilder::getIndexToNode() {
    return indexToNode;
}


void MapsBuilder::addNode(IR_PTR node) {
    indexToNode[index] = node;
    index++;
}

// CompUnitIR
void MapsBuilder::operator()(CompUnitIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

// FuncDecl IR
void MapsBuilder::operator()(FuncDeclIR &node) {
    this->addNameToCurrentIndex(node.getName());
    this->addNode(&node);

    this->visit_children(node);
};

// Expression IRs
void MapsBuilder::operator()(BinOpIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(CallIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(ConstIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(ESeqIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(MemIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(NameIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(TempIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

// Statement IRs
void MapsBuilder::operator()(CJumpIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(ExpIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(JumpIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(LabelIR &node) {
    this->addNameToCurrentIndex(node.getName());
    this->visit_children(node);
    this->addNode(&node);
};


void MapsBuilder::operator()(MoveIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(ReturnIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};

void MapsBuilder::operator()(SeqIR &node) {
    this->visit_children(node);
    this->addNode(&node);
};
