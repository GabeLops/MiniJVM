#ifndef MINIJVM_PHIINSTRUCTION_H
#define MINIJVM_PHIINSTRUCTION_H
#include <map>
#include "instruction.h"

// Phi function instuction

class PhiInstruction : public Instruction {
private:
    int originalLHS;
    int lhsVariable;
    std::map<int, int> *rhsVariableMap;

	Instruction *createStoreInstruction(int varNumber, int bytecodeIndex);

	Instruction *createLoadInstruction(int varNumber, int bytecodeIndex);

public:
	PhiInstruction(int variable);

	~PhiInstruction();

	int getOriginalLHS() const;

	int getCurrentLHS() const;

	void setCurrentLHS(int LHS);

	void setRHSVariable(int incomingBlockAddress, int value);

	int getRHSVariable(int incomingBlockAddress);

    std::map<int, int> *viewRHSMap();

	std::vector<Instruction *> getInstructions(int fromBlock);
};

#endif //MINIJVM_PHIINSTRUCTION_H
