#ifndef MINIJVM_RENAMEHELPER_H
#define MINIJVM_RENAMEHELPER_H

#include <vector>
#include <map>
// variable renumbering in SSA Form
class RenameHelper {
private:
    int counter;
    std::map<int, std::vector<int> *> variableStacks;

public:
    RenameHelper() : counter(1) { }

	int findVariableForNumber(int number);

	void pop(int variable);

	int top(int variable);

	void genName(int variable);
};

#endif //MINIJVM_RENAMEHELPER_H
