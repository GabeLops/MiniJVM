#ifndef MINIJVM_BASICBLOCK_H
#define MINIJVM_BASICBLOCK_H

#include <utility>
#include <vector>
#include "instruction.h"


//   block   class

 class BasicBlock {
private:
	int id;
    int startingAddress;

	int inStackDepth;

    std::vector<BasicBlock *> successors;
    std::vector<BasicBlock *> predecessors;
    std::vector<Instruction *> instructions;
public:
    BasicBlock(int startingAddress) : startingAddress(startingAddress) {
	}

	int getId()	{return id;	}
	void setId(int idx) { id = idx; }
	int getStackDepth() { return inStackDepth; }

    void addInstruction(Instruction *instruction) {
        instructions.emplace_back(instruction);
    }

    void addSuccessor(BasicBlock *block) {
        successors.emplace_back(block);
    }

    void addPredecessor(BasicBlock *block) {
        predecessors.emplace_back(block);
    }

    int getStartingAddress() const {
        return startingAddress;
    }

    std::vector<Instruction *> &getInstructions() {
        return instructions;
    }

    std::vector<BasicBlock *> &getPredecessors() {
        return predecessors;
    }

    std::vector<BasicBlock *> &getSuccessors() {
        return successors;
    }
};

#endif //MINIJVM_BASICBLOCK_H
