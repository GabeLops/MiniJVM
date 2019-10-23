#include "Instruction.h"
#include "PhiInstruction.h"
#include "Opcodes.h"

PhiInstruction::PhiInstruction(int variable) : Instruction(-1, phi_b), lhsVariable(variable), originalLHS(variable) {
	rhsVariableMap = new std::map<int, int>();
}

PhiInstruction::~PhiInstruction() {
	delete rhsVariableMap;
}

int PhiInstruction::getOriginalLHS() const { return originalLHS; }

int PhiInstruction::getCurrentLHS() const { return lhsVariable; }

void PhiInstruction::setCurrentLHS(int LHS) { lhsVariable = LHS; }

void PhiInstruction::setRHSVariable(int incomingBlockAddress, int value) {
	(*rhsVariableMap)[incomingBlockAddress] = value;
}

int PhiInstruction::getRHSVariable(int incomingBlockAddress) {
	return (*rhsVariableMap)[incomingBlockAddress];
}

std::map<int, int> *PhiInstruction::viewRHSMap() { return rhsVariableMap; };

Instruction *PhiInstruction::createStoreInstruction(int varNumber, int bytecodeIndex) {
	if (varNumber == 0)
		return new Instruction(bytecodeIndex, istore_0_b);
	else if (varNumber == 1)
		return new Instruction(bytecodeIndex, istore_1_b);
	else if (varNumber == 2)
		return new Instruction(bytecodeIndex, istore_2_b);
	else if (varNumber == 3)
		return new Instruction(bytecodeIndex, istore_3_b);
	else {
		std::vector<unsigned char> operands;
		operands.emplace_back((unsigned char)varNumber);
		return new Instruction(bytecodeIndex, istore_b, operands);
	}
}

Instruction *PhiInstruction::createLoadInstruction(int varNumber, int bytecodeIndex) {
	if (varNumber == 0)
		return new Instruction(bytecodeIndex, iload0_b);
	else if (varNumber == 1)
		return new Instruction(bytecodeIndex, iload1_b);
	else if (varNumber == 2)
		return new Instruction(bytecodeIndex, iload2_b);
	else if (varNumber == 3)
		return new Instruction(bytecodeIndex, iload3_b);
	else {
		std::vector<unsigned char> operands;
		operands.emplace_back((unsigned char)varNumber);
		return new Instruction(bytecodeIndex, iload_b, operands);
	}
}

std::vector<Instruction *> PhiInstruction::getInstructions(int fromBlock) {
	std::vector<Instruction *> result;
	if (rhsVariableMap->count(fromBlock) > 0) {
		result.emplace_back(createLoadInstruction((*rhsVariableMap)[fromBlock], getByteCodeIndex()));
		result.emplace_back(createStoreInstruction(getCurrentLHS(), getByteCodeIndex()));
	}
	return result;
}