
#include <windows.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include "Parser.h"
#include "Interpreter.h"
#include "SSAgenerator.h"



using namespace std;

int main(int argc, char* argv[])
{
	{

			printf("====================  Parser  ==================\n");					
			
		    Parser parser(std::string("C:/Users/Irvine/Documents/testCases/Test2") + ".class");
//			Parser parser(std::string(*(argv + 1)) + ".class");	 
			parser.parse();

			printf("==================== Interpret ===================\n\n");

			
			Interpreter interpreter(parser.constPool, parser.methods);
			interpreter.interpret();

			printf("====================  SSA  ===================\n\n");

			
			SSAgenerator sSAgenerator1(parser.constPool, parser.methods);
			sSAgenerator1.generate();

//		}
	}
}