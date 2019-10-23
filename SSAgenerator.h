
#ifndef MINIJVM_SSAGENERATOR_H
#define MINIJVM_SSAGENERATOR_H

#include "Opcodes.h"
#include "Method.h"
#include "ConstPoolEntry.h"
#include <vector>

//   SSA  generate

class SSAgenerator
{
	std::vector<ConstPoolEntry*> constPool;
	std::vector<Method> methods;

public:
	SSAgenerator(std::vector<ConstPoolEntry*> constPool, std::vector<Method> methods) :
		constPool(constPool), methods(methods) {/*TODO: deep copy these vectors*/};//constPool : bytecodes  , methods : functions
	~SSAgenerator();
	void generate();	// SSA generator
	void generateForMethod(Method* method);  // SSA Form make according to Method
	void printSSAassign(Method* method);	//  
	void printSSA(Method* method, std::set<int> &visitedSet);  //SSA Form instruction to Assamble Form Printing
	Method getMethod(std::string name);		//get Method instance according to  function name

};
#endif//MINIJVM_SSAGENERATOR_H

