#ifndef UTILS_H
#define UTILS_H

#define STATUS_BIT(x) getStatusBit(cpu, x)

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int DWord;

typedef unsigned char OpCode;
typedef signed short Cycles;

typedef enum {
	invalid, a, absIndexIndirectX, absIndexX, absIndexY, absIndirect, A,
	immediate, implied, PCRelative, stack, zp, zpIndexIndirectX,
	zpIndexX, zpIndexY, zpIndirect, zpIndexIndirectY
}AddressMode;

typedef enum {
	ADC, AND, ASL, BBR, BBS, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRA, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP, JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP,
	PHX, PHY, PLA, PLP, PLX, PLY, RMB, ROL, ROR, RTI, RTS, SBC, SEC, SED, SEI, SMB, STA, STP, STX, STY, STZ, TAX, TAY, TRB, TSB, TSX, TXA, TXS, TYA, WAI
}Instruction;

typedef enum {
	carry = 1, zero = 2, irqbDisable = 4, decimalMode = 8, brkCommand = 16, custom = 32, overflow = 64, negative = 128
}StatusBit;

Word combineLoHiByte(Byte loByte, Byte hiByte) {
	return (hiByte << 8) | loByte;
}


//void printAddrModeNames(Byte high, Byte low){
//	switch (getAddressMode((high << 4) | low)) {
//	case 1: printf("%8s", "a"); break;
//	case 2: printf("%8s", "(a,x)"); break;
//	case 3: printf("%8s", "a,x"); break;
//	case 4: printf("%8s", "a,y"); break;
//	case 5: printf("%8s", "(a)"); break;
//	case 6: printf("%8s", "A"); break;
//	case 7: printf("%8s", "#"); break;
//	case 8: printf("%8s", "i"); break;
//	case 9: printf("%8s", "r"); break;
//	case 10: printf("%8s", "s"); break;
//	case 11: printf("%8s", "zp"); break;
//	case 12: printf("%8s", "(zp,x)"); break;
//	case 13: printf("%8s", "zp,x"); break;
//	case 14: printf("%8s", "zp,y"); break;
//	case 15: printf("%8s", "(zp)"); break;
//	case 16: printf("%8s", "(zp),y"); break;
//	default: printf("%8s", "-");  break;
//	}
//}

#endif
