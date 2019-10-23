#include "Rename.h"


int RenameHelper::findVariableForNumber(int number) {
	for (auto entry : variableStacks) {
		for (int val : *entry.second) {
			if (val == number)
				return entry.first;
		}
	}
	return -1;
}

void RenameHelper::pop(int variable) {
	if (variableStacks.count(variable) != 0) {
		std::vector<int> *p = variableStacks[variable];
		p->pop_back();
		if (p->empty()) {
			delete p;
			variableStacks.erase(variable);
		}
	}
}

int RenameHelper::top(int variable) {
	if (variableStacks.count(variable) == 0)
		genName(variable);

	return variableStacks[variable]->back();
}

void RenameHelper::genName(int variable) {
	if (variableStacks.count(variable) == 0)
		variableStacks[variable] = new std::vector<int>();

	variableStacks[variable]->push_back(counter);
	counter++;
}