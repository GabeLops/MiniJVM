#ifndef MINIJVM_INSTRUCTION_H
#define MINIJVM_INSTRUCTION_H

#include <vector>
#include "Opcodes.h"


// bytecode   instruction

class Instruction {
private:
	int type;
    int byteCodeIndex;
    unsigned char opCode;
    std::vector<unsigned char> operands;
public:
    explicit Instruction(int byteCodeIndex, unsigned char op_code, std::vector<unsigned char> operands)
            : byteCodeIndex(byteCodeIndex), opCode(op_code), operands(std::move(operands)) {
			switch (op_code) {
				case iload_b:
				case istore_b:
				case bipush_b:
					type = twobyte;
					break;
				case	iinc_b:
				case	invokestatic_b:
				case	invokevirtual_b:
				case	getstatic_b:
					type = threebyte;
					break;
				case 	if_icmpne_b:
				case	if_icmpeq_b:
				case	if_icmpgt_b:
				case	if_icmpge_b:
				case	if_icmplt_b:
				case	if_icmple_b:
				case	ifeq_b:
				case	ifne_b:
				case	ifgt_b:
				case	ifge_b:
				case	iflt_b:
				case	ifle_b:
					type = condition;
					break;
			}
	}

    explicit Instruction(int byteCodeIndex, unsigned char op_code)
            : byteCodeIndex(byteCodeIndex), opCode(op_code) {   
			switch (op_code) {
			case	iconst_m1_b:
			case	iconst_0_b:
			case	iconst_1_b:
			case	iconst_3_b:
			case	iconst_4_b:
			case	iconst_5_b:
				type = const_i;
				break;
			case	iload0_b:
			case	iload1_b:
			case	iload2_b:
			case	iload3_b:
				type = load_i;
				break;
			case	istore_0_b:
			case	istore_1_b:
			case	istore_2_b:
			case	istore_3_b:
				type = store_i;
			case	return_b:
			case	ireturn_b:
				type = return_i;
				break;
			case	iadd_b:
			case	isub_b:
			case	imul_b:
			case	idiv_b:
			case	ishl_b:
			case	ishr_b:
				type = math;
				break;
		}
	}

    virtual ~Instruction() {}

    int getOpCode() const {
        return opCode;
    }

    std::vector<unsigned char> &getOperands() {
        return operands;
    }

    int getByteCodeIndex() const {
        return byteCodeIndex;
    }
	int getType() { return type; }
};

#endif //MINIJVM_INSTRUCTION_H
