
#ifndef MINIJVM_CONSTANTS_H
#define MINIJVM_CONSTANTS_H

#include <string>



//java  bytecode Opcode 

//constant pool tag bytes
const int strTag = 1;
const int intTag = 3;
const int floatTag = 4;
const int longTag = 5;
const int doubleTag = 6;
const int classRefTag = 7;
const int strRefTag = 8;
const int fieldRefTag = 9;
const int methodRefTag = 10;
const int interfaceMethodRefTag = 11;
const int nameTypeDescriptorTag = 12;
const int methodHandleTag = 15;
const int methodTypeTag = 16;
const int dynamicTag = 17;
const int invokeDynamicTag = 18;
const int moduleTag = 19;
const int packageTag = 20;


//bytecode instructions
//"_b" means byte, make sure not to conflict with function names
//store and load
const uint8_t iconst_m1_b = 0x02;
const uint8_t iconst_0_b = 0x03;
const uint8_t iconst_1_b = 0x04;
const uint8_t iconst_2_b = 0x05;
const uint8_t iconst_3_b = 0x06;	
const uint8_t iconst_4_b = 0x07;
const uint8_t iconst_5_b = 0x08;
const uint8_t iload_b = 0x15;
const uint8_t iload0_b = 0x1a;
const uint8_t iload1_b = 0x1b;
const uint8_t iload2_b = 0x1c;
const uint8_t iload3_b = 0x1d;
const uint8_t istore_b = 0x36;
const uint8_t istore_0_b = 0x3b;
const uint8_t istore_1_b = 0x3c;
const uint8_t istore_2_b = 0x3d;
const uint8_t istore_3_b = 0x3e;
const uint8_t aload0_b = 42;

//arithmetic
const uint8_t iadd_b = 0x60;
const uint8_t iinc_b = 0x84;
const uint8_t isub_b = 0x64;
const uint8_t imul_b = 0x68;
const uint8_t idiv_b = 0x6c;
const uint8_t ishl_b = 0x78;
const uint8_t ishr_b = 0x7a;
//conditionals
const uint8_t if_icmpne_b = 0xa0;
const uint8_t if_icmpeq_b = 0x9f;
const uint8_t if_icmpgt_b = 0xa3;
const uint8_t if_icmpge_b = 0xa2;
const uint8_t if_icmplt_b = 0xa1;
const uint8_t if_icmple_b = 0xa4;
const uint8_t ifeq_b = 0x99;
const uint8_t ifne_b = 0x9a;
const uint8_t ifgt_b = 0x9d;
const uint8_t ifge_b = 0x9c;
const uint8_t iflt_b = 0x9b;
const uint8_t ifle_b = 0x9e;
//other
const uint8_t goto_b = 0xa7;
const uint8_t bipush_b = 0x10;
const uint8_t invokestatic_b = 0xb8;
const uint8_t invokevirtual_b = 0xb6;

const uint8_t invokespecial_b = 0xb7;
const uint8_t op_getstatic_b = 0xb2;
const uint8_t return_b = 0xb1;
const uint8_t ireturn_b = 0xac;
//skip over these
const uint8_t getstatic_b = 0xb2;
const uint8_t phi_b = (unsigned char)255;


//Opcode string getting

static std::string getStringFromOpCode(int opCode)
{
	switch (opCode) {
	case iconst_m1_b:
		return "iconst_ml";
	case iconst_0_b:
		return "iconst_0";
	case iconst_1_b:
		return "iconst_1";
	case iconst_2_b:
		return "iconst_2";
	case iconst_3_b:
		return "iconst_3";
	case iconst_4_b:
		return "iconst_4";
	case iconst_5_b:
		return "iconst_5";
	case iload_b:
		return "iload";
	case iload0_b:
		return "iload_0";
	case iload1_b:
		return "iload_1";
	case iload2_b:
		return "iload_2";
	case iload3_b:
		return "iload_3";
	case istore_b:
		return "istore";
	case istore_0_b:
		return "istore_0";
	case istore_1_b:
		return "istore_1";
	case istore_2_b:
		return "istore_2";
	case istore_3_b:
		return "istore_3";
	case iadd_b:
		return "iadd";
	case if_icmpeq_b:
		return "if_icmpeq";
	case if_icmpne_b:
		return "if_icmpne";
	case if_icmplt_b:
		return "if_icmplt";
	case if_icmpge_b:
		return "if_icmpge";
	case if_icmpgt_b:
		return "if_icmpgt";
	case if_icmple_b:
		return "if_icmple";
	case ifeq_b:
		return "ifeq";
	case ifne_b:
		return "ifne";
	case iflt_b:
		return "iflt";
	case ifge_b:
		return "ifge";
	case ifgt_b:
		return "ifgt";
	case ifle_b:
		return "ifle";
	case invokespecial_b:
		return "invokespecial";
	case invokestatic_b:
		return "invokestatic";
	case invokevirtual_b:
		return "invokevirtual";
	case getstatic_b:
		return "getstatic";
	case goto_b:
		return "goto";
	case return_b:
		return "return";
	case aload0_b:
		return "aload_0";
	case bipush_b:
		return "bipush";
	case iinc_b:
		return "iinc";
	case phi_b:
		return "phi";
	default:
		return "Unknown opcode";
	}
}


enum{const_i,store_i,load_i,return_i,twobyte,threebyte,math,condition};

#endif//MINIJVM_CONSTANTS_H