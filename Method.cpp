#include "Method.h"
#include <string>
#include <utility>
#include <iterator> 
#include <set>
#include <stack>
#include <map>
#include <algorithm>
#include <queue>
#include "Opcodes.h"

//possible solution: implement deep copy of vector and Attrs
//this will involve overriding for each Attr subclass
Method::~Method()
{

}

std::string Method::getName()
{
	return name;
}

std::string Method::getDescriptor()
{
	return descriptor;
}

unsigned short Method::getMaxLocals()
{
	return maxLocals;
}

int Method::getByteCodeSize()
{
	return byteCodes.size();
}

uint8_t Method::getByte(int index)
{
	if (index >= byteCodes.size())
	{
		//printf("attmped to access byte code with index %d when the number of byte codes is %d\n", index, byteCodes.size());
		exit(1);
	}
	return byteCodes[index];
}
unsigned char* Method::getByteCodes() {
	unsigned char* bytecodes = new unsigned char[getByteCodeSize()];
	for (int i = 0; i < getByteCodeSize(); i++)
		bytecodes[i] = getByte(i);
	return bytecodes;
}

int16_t Method::getTwoByte(int index)
{
	uint8_t a = getByte(index);
	uint8_t b = getByte(index + 1);
	return (uint16_t) a << 8 | b;
}


void Method::reversePostOrder(BasicBlock *block, std::stack<BasicBlock *> &reversePost, std::set<int> &visited) {
	visited.insert(block->getStartingAddress());
	for (auto b : block->getSuccessors()) {
		if (visited.count(b->getStartingAddress()) == 0)
			reversePostOrder(b, reversePost, visited);
	}
	reversePost.push(block);
}

std::set<BasicBlock *> *Method::computeBlocksDominatedBy(std::map<int, std::set<BasicBlock *> *> &dominatorTree,
	BasicBlock *dominatedBy) {
	std::set<BasicBlock *> *blocks = new std::set<BasicBlock *>();
	for (auto &it : dominatorTree) {
		if (it.second->count(dominatedBy) > 0)
			blocks->insert(basicBlocks[it.first]);
	}
	return blocks;
}

void Method::createSSA() {
	std::stack<BasicBlock *> reversePostOrderStack;
	std::vector<BasicBlock *> reversPostOrderList;
	std::set<int> visited;
	reversePostOrder(basicBlocks[0], reversePostOrderStack, visited);
	while (!reversePostOrderStack.empty()) {
		reversPostOrderList.emplace_back(reversePostOrderStack.top());
		reversePostOrderStack.pop();
	}

	std::map<int, std::set<BasicBlock *> *> dominatorTree;
	auto initial = new std::set<BasicBlock *>();
	initial->insert(basicBlocks[0]);
	dominatorTree[0] = initial;

	auto allBlocks = new std::set<BasicBlock *>();
	for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
		allBlocks->insert(it->second);
	}
	for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
		BasicBlock *b = it->second;
		if (b->getStartingAddress() != 0) {
			dominatorTree[b->getStartingAddress()] = new std::set<BasicBlock *>(*allBlocks);
		}
	}

	bool changed = true;
	while (changed) {
		changed = false;
		for (auto block : reversPostOrderList) {
			std::set<BasicBlock *> *newSet = nullptr;
			auto predecessors = block->getPredecessors();
			if (!predecessors.empty()) {
				newSet = new std::set<BasicBlock *>(*dominatorTree[predecessors[0]->getStartingAddress()]);
				for (int i = 1; i < predecessors.size(); i++) {
					auto other = dominatorTree[predecessors[i]->getStartingAddress()];
					auto intersection = new std::set<BasicBlock *>();
					set_intersection(newSet->begin(), newSet->end(), other->begin(), other->end(),
						std::inserter(*intersection, intersection->begin()));
					delete newSet;
					newSet = intersection;
				}
			}

			if (newSet == nullptr)
				newSet = new std::set<BasicBlock *>();

			newSet->insert(block);
			if (*newSet != *dominatorTree[block->getStartingAddress()]) {
				delete dominatorTree[block->getStartingAddress()];
				dominatorTree[block->getStartingAddress()] = newSet;
				changed = true;
			}
			else
				delete newSet;
		}
	}

	std::map<int, std::set<BasicBlock *> *> dominanceFrontier;
	std::map<int, BasicBlock *> idom;

	//Compute immediate dominators
	for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
		BasicBlock *b = it->second;
		//Starting block has no immediate dominator
		if (b->getStartingAddress() != 0) {
			for (BasicBlock *dominator : *dominatorTree[b->getStartingAddress()]) {
				if (b->getStartingAddress() != dominator->getStartingAddress()) {
					bool isIdom = true;
					for (BasicBlock *otherDominator : *dominatorTree[b->getStartingAddress()]) {
						if (otherDominator->getStartingAddress() != b->getStartingAddress() &&
							otherDominator->getStartingAddress() != dominator->getStartingAddress()) {
							if (dominatorTree[otherDominator->getStartingAddress()]->count(dominator) > 0) {
								isIdom = false;
								break;
							}
						}
					}

					if (isIdom) {
						idom[b->getStartingAddress()] = dominator;
						break;
					}
				}
			}
		}
	}

	//Every node is in its own dominance frontier so initialize the frontiers with themselves
	for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
		BasicBlock *b = it->second;
		auto s = new std::set<BasicBlock *>();
		//s->insert(b);
		dominanceFrontier[b->getStartingAddress()] = s;
	}

	for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
		BasicBlock *b = it->second;
		if (b->getPredecessors().size() >= 2) {
			for (auto p : b->getPredecessors()) {
				BasicBlock *runner = p;
				while (runner->getStartingAddress() != idom[b->getStartingAddress()]->getStartingAddress()) {
					dominanceFrontier[runner->getStartingAddress()]->insert(b);
					runner = idom[runner->getStartingAddress()];
				}
			}
		}
	}

	//		        printf("Dominance tree for method %s\n", this->name.c_str());
	//		        for (auto &it : dominatorTree) {
	//		            int blockAddress = it.first;
	//		            for (auto dominator : *it.second) {
	//		                printf("Block %d is dominated by block %d\n", blockAddress,
	//		                       dominator->getStartingAddress());
	//		            }
	//		        }

	//		        printf("Immediate dominators for method %s\n", this->name.c_str());
	//		        for (auto &it : idom) {
	//		            int blockAddress = it.first;
	//		            printf("Block %d is the immediate dominator of Block %d\n", it.second->getStartingAddress(),
	//		                   blockAddress);
	//		        }


	//		        printf("Dominance frontier for method %s\n", this->name.c_str());
	//		        for (auto &it : dominanceFrontier) {
	//		            int blockAddress = it.first;
	//		            for (auto dominator : *it.second) {
	//		                printf("Block %d is in the dominance frontier of Block %d\n", dominator->getStartingAddress(),
	//		                       blockAddress);
	//		            }
	//		        }

					//Now we place the phi functions accordingly
	for (int variable = 0; variable < this->getMaxLocals(); variable++) {//			this->code->getMaxLocals(); indtead of maxLocals

		std::queue<int> worklist;
		std::set<int> everOnWorklist;
		std::set<int> alreadyHasPhiFunc;
		//Add all blocks containing an assignment to the worklist and to the ever on worklist
		for (auto block : getAllBlocksWithAssignment(variable)) {
			worklist.push(block->getStartingAddress());
			everOnWorklist.insert(block->getStartingAddress());
		}

		while (!worklist.empty()) {
			BasicBlock *n = basicBlocks[worklist.front()];
			worklist.pop();

			for (BasicBlock *d : *dominanceFrontier[n->getStartingAddress()]) {
				if (alreadyHasPhiFunc.count(d->getStartingAddress()) == 0) {

					d->getInstructions().insert(d->getInstructions().begin(), new PhiInstruction(variable));

					//		         printf("Adding phi instruction to block %d from block %d for variable %d \n",
					//			                        d->getStartingAddress(),
					//			                        n->getStartingAddress(), variable);

					alreadyHasPhiFunc.insert(d->getStartingAddress());
					if (everOnWorklist.count(d->getStartingAddress()) == 0) {
						worklist.push(d->getStartingAddress());
						everOnWorklist.insert(d->getStartingAddress());
					}
				}
			}
		}
	}

		//Now we go through and rename all the variables (actually we create new ones)
		std::set<int> visitedRenaming;
		RenameHelper helper;
		rename(basicBlocks[0], dominatorTree, visitedRenaming, helper); 
}

Instruction * Method::replaceLHS(Instruction *instruction, int oldLHS, RenameHelper &helper) {
	return createStoreInstruction(helper.top(oldLHS), instruction->getByteCodeIndex());
}

Instruction* Method::replaceRHS(Instruction *instruction, int oldRHS, RenameHelper &helper) {
	return createLoadInstruction(helper.top(oldRHS), instruction->getByteCodeIndex());
}

void Method::rename(BasicBlock *block, std::map<int, std::set<BasicBlock *> *> &dominanceTree,
	std::set<int> &visited, RenameHelper &helper) {
	if (visited.count(block->getStartingAddress()) != 0)
		return;

	visited.insert(block->getStartingAddress());
	

	for (Instruction *instruction : block->getInstructions()) {
		if (instruction->getOpCode() != phi_b)
			break;

		PhiInstruction *phi = dynamic_cast<PhiInstruction *>(instruction);
		helper.genName(phi->getCurrentLHS());
		phi->setCurrentLHS(helper.top(phi->getCurrentLHS()));
	}

	for (int i = 0; i < block->getInstructions().size(); i++) {
		Instruction *instruction = block->getInstructions()[i];
		if (instruction->getOpCode() == phi_b)
			continue; //Skip phis when we are processing instructions because we are processing "statements"

		int rhs = getRHSVariable(instruction);
		if (rhs != -1) {
			Instruction *inst = replaceRHS(instruction, rhs, helper);
			delete instruction;
			instruction = inst;
			block->getInstructions()[i] = inst;
		}

		int lhs = getLHSVariable(instruction);
		if (lhs != -1) {
			helper.genName(lhs);
			Instruction *inst = replaceLHS(instruction, lhs, helper);
			delete instruction;
			block->getInstructions()[i] = inst;
		}
	}

	for (BasicBlock *s : block->getSuccessors()) {
		for (Instruction *instruction : s->getInstructions()) {
			if (instruction->getOpCode() != phi_b)
				break;

			PhiInstruction *phi = dynamic_cast<PhiInstruction *>(instruction);
			phi->setRHSVariable(block->getStartingAddress(),
				helper.top(helper.findVariableForNumber(phi->getOriginalLHS())));
		}
	}

	std::set<BasicBlock *> *dominatedBy = computeBlocksDominatedBy(dominanceTree, block);
	for (BasicBlock *s : *dominatedBy) {
		rename(s, dominanceTree, visited, helper);
	}
	delete dominatedBy;

	for (Instruction *instruction : block->getInstructions()) {
		if (instruction->getOpCode() == phi_b) {
			PhiInstruction *phi = dynamic_cast<PhiInstruction *>(instruction);
			helper.pop(helper.findVariableForNumber(phi->getCurrentLHS()));
		}
		else {
			int lhs = getLHSVariable(instruction);
			if (lhs != -1) {
				helper.pop(helper.findVariableForNumber(lhs));
			}
		}
	}
}


int Method::getLHSVariable(Instruction *instruction) {
	if (instruction->getOpCode() == istore_0_b)
		return 0;
	else if (instruction->getOpCode() == istore_1_b)
		return 1;
	else if (instruction->getOpCode() == istore_2_b)
		return 2;
	else if (instruction->getOpCode() == istore_3_b)
		return 3;
	else if (instruction->getOpCode() == istore_b)
		return instruction->getOperands()[0];
	else
		return -1;
}

int Method::getRHSVariable(Instruction *instruction) {
	if (instruction->getOpCode() == iload0_b)
		return 0;
	else if (instruction->getOpCode() == iload1_b)
		return 1;
	else if (instruction->getOpCode() == iload2_b)
		return 2;
	else if (instruction->getOpCode() == iload3_b)
		return 3;
	else if (instruction->getOpCode() == iload_b)
		return instruction->getOperands()[0];
	else
		return -1;
}

bool Method::containsAssignment(BasicBlock *block, int variableNumber) {
	for (auto &instruction : block->getInstructions()) {
		if (instruction->getOpCode() == istore_0_b) {
			if (variableNumber == 0)
				return true;
		}
		else if (instruction->getOpCode() == istore_1_b) {
			if (variableNumber == 1)
				return true;
		}
		else if (instruction->getOpCode() == istore_2_b) {
			if (variableNumber == 2)
				return true;
		}
		else if (instruction->getOpCode() == istore_3_b) {
			if (variableNumber == 3)
				return true;
		}
		else if (instruction->getOpCode() == istore_b || instruction->getOpCode() == iinc_b) {
			int var = (int)instruction->getOperands()[0];
			if (var == variableNumber)
				return true;
		}
	}
	return false;
}

std::vector<BasicBlock *> Method::getAllBlocksWithAssignment(int variableNumber) {
	std::vector<BasicBlock *> blocks;
	for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
		if (containsAssignment(it->second, variableNumber))
			blocks.emplace_back(it->second);
	}
	return blocks;
}

void Method::parseByteCode(unsigned char *byteCode, int length) {
	for (int i = 0; i < length; i++) {
		unsigned char opCode = byteCode[i];
//		printf("%d  %s\n",i, getStringFromOpCode((int)opCode).c_str());
		switch (opCode) {
			case iconst_m1_b:
			case iconst_0_b:
			case iconst_1_b:
			case iconst_2_b:
			case iconst_3_b:
			case iconst_4_b:
			case iconst_5_b:
			case return_b:
			case iadd_b:
			case aload0_b:
			case iload0_b:
			case iload1_b:
			case iload2_b:
			case iload3_b:
			case istore_0_b:
			case istore_1_b:
			case istore_2_b:
			case istore_3_b: {
				instructions.emplace_back(Instruction(i, opCode));
				break;
			}
			case iload_b:
			case istore_b:
			case bipush_b: {
				int codeIndex = i;
				i++;
				unsigned char index = byteCode[i];
				std::vector<unsigned char> operands;
				operands.emplace_back(index);
				instructions.emplace_back(Instruction(codeIndex, opCode, operands));
				break;
			}
			case if_icmpne_b:
			case if_icmpeq_b:
			case if_icmpgt_b:
			case if_icmplt_b:
			case if_icmpge_b:
			case if_icmple_b:
			case ifeq_b:
			case ifne_b:
			case ifgt_b:
			case iflt_b:
			case ifle_b:
			case ifge_b:
			case goto_b:
			case invokestatic_b:
			case invokespecial_b:
			case getstatic_b:
			case invokevirtual_b:
			case iinc_b: {
				int codeIndex = i;
				i++;
				unsigned char branch1 = byteCode[i];
				i++;
				unsigned char branch2 = byteCode[i];
				std::vector<unsigned char> operands;
				operands.emplace_back(branch1);
				operands.emplace_back(branch2);
				instructions.emplace_back(Instruction(codeIndex, opCode, operands));
				break;
			}
			default: {
				printf("Unknown opCode: %x \n", opCode);
			}
		}
	}
}

void Method::BlockNumbering()
{

	block_id = 0;
	for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it)
	{
		BasicBlock* block = it->second;
		block->setId(block_id++);
	}

	if (basicBlocks.size() > 0)
	{
		for (auto it = basicBlocks.begin(); it != basicBlocks.end(); )
		{
			BasicBlock* block = it->second;
			if (++it != basicBlocks.end())
			{
				BasicBlock* block1 = it->second;
				int end_address = block1->getStartingAddress() - 1;

				if (block->getInstructions().back()->getByteCodeIndex() > end_address)
				{
					std::vector<Instruction *>::iterator that;

					for (that = block->getInstructions().begin(); that != block->getInstructions().end();) {
						Instruction* inst = (Instruction*)*that;
						if (inst->getByteCodeIndex() > end_address)
						{
							delete inst;
							that = block->getInstructions().erase(that);
						}
						else
						{
							++that;
						}
					}
				}
			}
		}
	}

}

void Method::BuildSSA() {  //

	parseByteCode(getByteCodes(), getByteCodeSize());  // instruction array  generation from byt code

	buildBasicBlocks(0);	//block generation according to goto, if,and so on

	BlockNumbering();   //  block nubering and instruction rearrange

	linkBasicBlocks();  //block link, successor, Predecessor link
	
	createSSA();  // dominate  and  variable rename


}

void Method::linkBasicBlocks()
{
	for (auto it = basicBlocks.begin(); it != basicBlocks.end(); ++it)
	{
		BasicBlock *block = it->second;
		Instruction *instruction = block->getInstructions().back();

		switch (instruction->getOpCode()) {
			case goto_b: 
			{
				int jumpToAddress = instruction->getByteCodeIndex() +
					constructOffset(instruction->getOperands()[0], instruction->getOperands()[1]);
				BasicBlock *successor = findBasicBlockWithAddress(jumpToAddress);
				if (successor == nullptr)
					printf("Attempted to find a basic block with address that isn't valid from goto");
				else
				{
					successor->addPredecessor(block);
					block->addSuccessor(successor);
				}
				break;
			}
			case return_b:
			case ireturn_b:
			case invokevirtual_b:
			case invokestatic_b:
			case invokespecial_b:
				break;
			case if_icmpeq_b:
			case if_icmpne_b:
			case if_icmplt_b:
			case if_icmpge_b:
			case if_icmpgt_b:
			case if_icmple_b:
			case ifeq_b:
			case ifne_b:
			case iflt_b:
			case ifge_b:
			case ifgt_b:
			case ifle_b:
			{
				int jumpToAddress = instruction->getByteCodeIndex() +
					constructOffset(instruction->getOperands()[0], instruction->getOperands()[1]);
				BasicBlock *successor1 = findBasicBlockWithAddress(jumpToAddress);
				if (successor1 == nullptr)
					printf("Attempted to find a basic block with address that isn't valid for jump");
				else
				{
					successor1->addPredecessor(block);
					block->addSuccessor(successor1);
				}

				int index = getIndexOfByteCode(instructions, instruction->getByteCodeIndex()) + 1;
				if (index < instructions.size())
				{
					BasicBlock *successor2 = findBasicBlockWithAddress(instructions[index].getByteCodeIndex());
					if (successor2 == nullptr)
						printf("Attempted to find a basic block with address that isn't valid for fallthrough");
					else
					{
						successor2->addPredecessor(block);
						block->addSuccessor(successor2);
					}
				}
				break;
			}
			default:
				if (instruction == block->getInstructions().back())
				{
					for (auto that = it; that != basicBlocks.end(); ++that)
					{
						BasicBlock *successor = that->second;
						if (block->getId() + 1 == successor->getId())
						{
							successor->addPredecessor(block);
							block->addSuccessor(successor);

						}
					}
				}
				break;	
		}
	}
	
}

short Method::constructOffset(unsigned char one, unsigned char two) {//
	return (short)(((unsigned short)(one << 8)) | two);
}

int Method::getIndexOfByteCode(std::vector<Instruction> &instructions, int byteCodeIndex) {//static
	for (int i = 0; i < instructions.size(); i++) {
		if (instructions[i].getByteCodeIndex() == byteCodeIndex)
			return i;
	}
	return -1;
}

BasicBlock *Method::findBasicBlockWithAddress(int address) {
	return basicBlocks[address];
}
Instruction *Method::createLoadInstruction(int varNumber, int bytecodeIndex) {
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

Instruction *Method::createStoreInstruction(int varNumber, int bytecodeIndex) {
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

std::vector<Instruction *> Method::incInstructionAlternative(unsigned char varNumber, unsigned char value, int bytecodeIndex) {
	std::vector<Instruction *> vec;
	std::vector<unsigned char> operands;
	operands.emplace_back(value);
	vec.emplace_back(new Instruction(bytecodeIndex, bipush_b, operands));
	vec.emplace_back(createLoadInstruction(varNumber, bytecodeIndex));
	vec.emplace_back(new Instruction(bytecodeIndex, iadd_b));
	vec.emplace_back(createStoreInstruction(varNumber, bytecodeIndex));
	return vec;
}

void Method::buildBasicBlocks(int instructionIndex) {
	BasicBlock *current = nullptr;

	for (; instructionIndex < instructions.size(); instructionIndex++) {
		Instruction instruction = instructions[instructionIndex];
		if (current == nullptr)
		{
			current = new BasicBlock(instruction.getByteCodeIndex());
		}

		if (instruction.getOpCode() == iinc_b) {
			auto vec = incInstructionAlternative(instruction.getOperands()[0], instruction.getOperands()[1],
				instruction.getByteCodeIndex());
			for (Instruction *v : vec) {
				current->addInstruction(v);
			}
		}
		else
		{ 
			current->addInstruction(new Instruction(instruction.getByteCodeIndex(),	(unsigned char)instruction.getOpCode(),instruction.getOperands()));		
		}
		
		switch (instruction.getOpCode()) {
			//case return_b:
			//case invokevirtual_b:
			//case invokestatic_b:
			//case invokespecial_b:

			case goto_b: {
				basicBlocks[current->getStartingAddress()] = current;
				current = nullptr;
				int jumpToAddress = instruction.getByteCodeIndex() +
					constructOffset(instruction.getOperands()[0], instruction.getOperands()[1]);
				if (findBasicBlockWithAddress(jumpToAddress) == nullptr) {
					buildBasicBlocks(getIndexOfByteCode(instructions, jumpToAddress));
				}
				return;
			}
			case if_icmpeq_b:
			case if_icmpne_b:
			case if_icmplt_b:
			case if_icmpge_b:
			case if_icmpgt_b:
			case if_icmple_b:
			case ifeq_b:
			case ifne_b:
			case iflt_b:
			case ifge_b:
			case ifgt_b:
			case ifle_b: {
				basicBlocks[current->getStartingAddress()] = current;
				current = nullptr;
				int jumpToAddress = instruction.getByteCodeIndex() +
					constructOffset(instruction.getOperands()[0], instruction.getOperands()[1]);
				if (findBasicBlockWithAddress(jumpToAddress) == nullptr) {
					buildBasicBlocks(getIndexOfByteCode(instructions, jumpToAddress));
				}

				int index = instructionIndex + 1;
				if (index < instructions.size()) {
					Instruction next = instructions[index];
					int fallThroughAddress = next.getByteCodeIndex();
					if (findBasicBlockWithAddress(fallThroughAddress) == nullptr) {
						buildBasicBlocks(getIndexOfByteCode(instructions, fallThroughAddress));
					}
				}
				return;
			}
			default: {
				//Do nothing unless it is a control flow instruction
				break;
			}
		}
	}

	if (current != nullptr)
		basicBlocks[current->getStartingAddress()] = current;
}

