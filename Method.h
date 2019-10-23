
#ifndef MINIJVM_METHOD_H
#define MINIJVM_METHOD_H

#include <vector>
#include <iostream>

#include <cstdint>

#include <utility>
#include <iterator> 
#include <set>
#include <stack>
#include <map>
#include <algorithm>
#include <queue>
#include "instruction.h"
#include "Basicblock.h"
#include "Opcodes.h"
#include "PhiInstruction.h"
#include "Rename.h"


//   According to function

class Method
{
private:
	int block_id;
	int accessFlags;
	std::string name;

	std::string descriptor;

	unsigned short maxStack;
	unsigned short maxLocals;
	std::vector<uint8_t> byteCodes;
 
public:	
	std::vector<Instruction> instructions;    // instruction array including in function
	std::map<int, BasicBlock *> basicBlocks;	// block array including in function

	std::string getName();		// function name
	std::string getDescriptor();
	//returns maxLocals
	unsigned short getMaxLocals();
	//returns the number of bytes in byteCodes

	int getByteCodeSize();  // ByteCode size

	//get the byte with the given index
	uint8_t getByte(int index);

	//get two bytes at given index and returns uint16 in big endian
	int16_t getTwoByte(int index);

	unsigned char* getByteCodes();   

	static short constructOffset(unsigned char one, unsigned char two);  //two char to one short (example  2byte operand )

	static int getIndexOfByteCode(std::vector<Instruction> &instructions, int byteCodeIndex); //  geting byte in byteCode  according to index


	void reversePostOrder(BasicBlock *block, std::stack<BasicBlock *> &reversePost, std::set<int> &visited);

	std::set<BasicBlock *> *computeBlocksDominatedBy(std::map<int, std::set<BasicBlock *> *> &dominatorTree,BasicBlock *dominatedBy);    // dominate tree computation 

	std::vector<BasicBlock *> getAllBlocksWithAssignment(int variableNumber);

	std::vector<Instruction *> incInstructionAlternative(unsigned char varNumber, unsigned char value, int bytecodeIndex);

	BasicBlock *findBasicBlockWithAddress(int address);

	Instruction *createLoadInstruction(int varNumber, int bytecodeIndex);

	Instruction *createStoreInstruction(int varNumber, int bytecodeIndex);

	void rename(BasicBlock *block, std::map<int, std::set<BasicBlock *> *> &dominanceTree, std::set<int> &visited, RenameHelper &helper);  // variable renaming

	int getLHSVariable(Instruction *instruction);  

	int getRHSVariable(Instruction *instruction);

	Instruction * replaceLHS(Instruction *instruction, int oldLHS, RenameHelper &helper);

	Instruction * replaceRHS(Instruction *instruction, int oldRHS, RenameHelper &helper);

	bool containsAssignment(BasicBlock *block, int variableNumber);


	void parseByteCode(unsigned char *byteCode, int length);// bytecode to instruction

	void BlockNumbering();  // block index numbering

	void createSSA();  // bytecode to instrution
	
	void BuildSSA();

	void linkBasicBlocks();   // link between blocks  , inserting phi functions 

	void buildBasicBlocks(int instructionIndex);

	Method(unsigned short accessFlags, std::string name, std::string descriptor, unsigned short maxStack, unsigned short maxLocals, std::vector<uint8_t> byteCodes) :
		accessFlags(accessFlags), name(name), descriptor(descriptor), maxStack(maxStack), maxLocals(maxLocals), byteCodes(byteCodes){
		block_id = 0;
	}

	~Method();

};
#endif//MINIJVM_METHOD_H