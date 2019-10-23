#include "SSAgenerator.h"
#include <string>
#include <utility>
#include <iterator> 
#include <set>
#include <stack>
#include <map>
#include <algorithm>
#include <queue>

SSAgenerator::~SSAgenerator()
{
}

void SSAgenerator::generate()
{	

	int i = 0;
	for (int i = 0; i < methods.size(); i++)
	{
		if (methods[i].getName() != "<init>")
		{ 
			generateForMethod(&methods[i]);
			printSSAassign(&methods[i]);
		}
	}
}


void SSAgenerator::generateForMethod(Method* method)
{
	if (method->getName() != "<init>") {
		method->BuildSSA();

//		printf("\n SSA of method:  %s \n", method->getName().c_str());
	}
}

void SSAgenerator::printSSAassign(Method* method) {
	std::set<int> visitedSet;
	printf("\n Method Name  %s\n", method -> getName().c_str());
	printSSA(method, visitedSet);
}

void SSAgenerator::printSSA(Method* method,  std::set<int> &visitedSet) {
	int jumpToAddress;
	int con = 0;
	int tempno = 0;
	int stackno = 0;

	BasicBlock* m_block;



	for (auto it = method->basicBlocks.begin(); it != method->basicBlocks.end(); ++it) {
		BasicBlock *block = it->second;
			printf("\n\n***** Block No #%d  *******\n\n", block->getId());

			for (Instruction *inst : block->getInstructions()) {
				auto cStr = getStringFromOpCode(inst->getOpCode()).c_str();
//				if (inst->getByteCodeIndex() > block->getEndingAddress()) break;
				switch (inst->getOpCode())
				{
				case iconst_0_b:std::cout << "MOV stack" << stackno++ << ",  const0" << std::endl; break;
				case iconst_1_b:std::cout << "MOV stack" << stackno++ << ",  const1" << std::endl; break;
				case iconst_2_b:std::cout << "MOV stack" << stackno++ << ",  const2" << std::endl; break;
				case iconst_3_b:std::cout << "MOV stack" << stackno++ << ",  const3" << std::endl; break;
				case iconst_4_b:std::cout << "MOV stack" << stackno++ << ",  const4" << std::endl; break;
				case iconst_5_b:std::cout << "MOV stack" << stackno++ << ",  const5" << std::endl; break;
					//case iconst_m1_b:std::cout << inst->getByteCodeIndex() << " : " << "MOV STACK" << stackno++ << " CONST" << con << std::endl; break;
				case iload0_b:std::cout << "MOV stack" << stackno++ << ", local0" << std::endl; break;
				case iload1_b:std::cout << "MOV stack" << stackno++ << ", local1" << std::endl; break;
				case iload2_b:std::cout << "MOV stack" << stackno++ << ", local2" << std::endl; break;
				case iload3_b:std::cout << "MOV stack" << stackno++ << ", local3" << std::endl; break;
				case istore_0_b:std::cout << "MOV local0,  stack" << --stackno << std::endl; break;
				case istore_1_b:std::cout << "MOV local1,  stack" << --stackno << std::endl; break;
				case istore_2_b:std::cout << "MOV local2,  stack" << --stackno << std::endl; break;
				case istore_3_b:std::cout << "MOV local3,  stack" << --stackno << std::endl; break;
				case istore_b:std::cout << "MOV local" << (int)inst->getOperands()[0] << " , stack" << --stackno << std::endl; break;
				case iload_b:std::cout << "MOV stack" << stackno++ << " , local" << (int)inst->getOperands()[0] << std::endl; break;
				case bipush_b:std::cout << "MOV stack" << stackno++ << " , const" << (int)inst->getOperands()[0] << std::endl; break;
				case iadd_b:std::cout << "ADD stack" << stackno - 1 << " , stack" << stackno - 1 << ", stack" << --stackno << std::endl; break;
				case goto_b:
					jumpToAddress = inst->getByteCodeIndex() + method->constructOffset(inst->getOperands()[0], inst->getOperands()[1]);
					m_block = method->findBasicBlockWithAddress(jumpToAddress);
					std::cout << "BRUNCOND  #" << m_block->getId() << std::endl;
					break;
				case return_b:
					std::cout << "RETURN " << std::endl; break;
					break;
				case if_icmpeq_b:
				case if_icmpne_b:
				case if_icmplt_b:
				case if_icmpge_b:
				case if_icmpgt_b:
				case if_icmple_b:
				case ifeq_b:
				case ifne_b:
				case ifge_b:
				case ifgt_b:
				case ifle_b:
					jumpToAddress = inst->getByteCodeIndex() + method->constructOffset(inst->getOperands()[0], inst->getOperands()[1]);
					m_block = method->findBasicBlockWithAddress(jumpToAddress);
					std::cout << "CMP temp" << tempno << " , stack" << stackno << " , stack" << stackno-- << std::endl;
					std::cout << "BRCOND  temp" << tempno++ << " , #" << m_block->getId();
					if (m_block->getSuccessors().size() > 0)
						std::cout << ", # " << m_block->getSuccessors().front()->getId() << std::endl;
					else std::cout << std::endl;
					break;
				default:
					break;
				}
				if (inst->getOpCode() == phi_b) {
					PhiInstruction *phi = dynamic_cast<PhiInstruction *>(inst);
					std::cout << "PHI local" << phi->getCurrentLHS() << " = [";
					for (auto &entry : *phi->viewRHSMap()) {
						m_block = method->findBasicBlockWithAddress(entry.first);
						std::cout << " local" << entry.second << "(#" << m_block->getId() << ") ";
					}
					std::cout << "]" << std::endl;
				}
				if (inst->getOpCode() == invokestatic_b) {
					ConstPoolEntry* methodRef = constPool[(int)method->constructOffset(inst->getOperands()[0], inst->getOperands()[1])];
					ConstPoolEntry* nameType = constPool[methodRef->getNameTypeIndex()];
					ConstPoolEntry* strRef = constPool[nameType->getNameIndex()];

					Method m = getMethod(strRef->getStr());
					std::cout << "CALL " << m.getName() << std::endl;

				}
				if (inst->getOpCode() == invokevirtual_b) {
					std::cout << "PRINT stack" << --stackno << std::endl;
				}
			}
	}
}

Method SSAgenerator::getMethod(std::string name)
{
	for (Method m : methods)
	{
		if (m.getName() == name)
		{
			return m;
		}
	}
	printf("method with name \"%s\" does not exist\n", name.c_str());
	exit(1);
}