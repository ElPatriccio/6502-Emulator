#ifndef INSTR_DECODER
#define INSTR_DECODER
#include"utils.h"

AddressMode getAddressMode(OpCode instr) {
	Byte lowerBits = instr & 0x0F;
	Byte higherBits = instr >> 4;

	switch (lowerBits) {
	case 0: 
		if (higherBits == 0 || higherBits == 4 || higherBits == 6)
			return stack;
		if (higherBits == 2)
			return a;
		if (higherBits % 2 == 1 || higherBits == 8)
			return PCRelative;
		if (higherBits > 9 && higherBits % 2 == 0)
			return immediate;
		break;
	case 1: 
		if (higherBits % 2 == 0)
			return zpIndexIndirectX;
		return zpIndexIndirectY;
	

	case 2: 
		if (higherBits == 10)
			return immediate;
		if (higherBits % 2 == 1)
			return zpIndirect;
		break;

	case 4: 
		if (higherBits == 4 || higherBits == 5 || higherBits == 13 || higherBits == 15)
			return invalid;
		if (higherBits < 3 || higherBits % 2 == 0)
			return zp;
		return zpIndexX;

	case 5: 
		if (higherBits % 2 == 0)
			return zp;
		return zpIndexX;

	case 6: 
		if (higherBits % 2 == 0)
			return zp;
		if (higherBits != 9 && higherBits != 11)
			return zpIndexX;
		return zpIndexY;

	case 7: 
		return zp;

	case 8: 
		if (higherBits % 2 == 0 && higherBits < 8)
			return stack;
		return implied;

	case 9: 
		if (higherBits % 2 == 0)
			return immediate;
		return absIndexY;

	case 10: 
		if (higherBits < 5 || higherBits == 6)
			return A;
		if (higherBits % 2 == 1 && higherBits != 9 && higherBits != 11)
			return stack;
		return implied;

	case 11: 
		if (higherBits == 12 || higherBits == 13)
			return implied; 
		break;

	case 12: 
		if (higherBits == 5 || higherBits == 13 || higherBits == 15)
			return invalid;
		if (higherBits < 5) {
			if (higherBits != 3)
				return a;
			return absIndexX;
		}
		if (higherBits == 6)
			return absIndirect;
		if (higherBits == 7)
			return absIndexIndirectX;
		if (higherBits > 7 && (higherBits % 2 == 0 || higherBits == 9))
			return a;
		return absIndexX;

	case 13: 
		if (higherBits % 2 == 0)
			return a;
		return absIndexX;

	case 14: 
		if (higherBits == 11)
			return absIndexY;
		if (higherBits % 2 == 0)
			return a;
		return absIndexX;

	case 15: return PCRelative;

	default: return invalid;
	}
	return invalid;
}

Instruction helperFunc1(Byte loBits, Byte hiBits) {
	if (hiBits < 2)
		return ORA;
	if (hiBits < 4)
		return AND;
	if (hiBits < 6)
		return EOR;
	if (hiBits < 8)
		return ADC;
	if (hiBits < 10)
		return loBits == 9 && hiBits == 8 ? BIT : STA;
	if (hiBits < 12)
		return LDA;
	if (hiBits < 14)
		return CMP;
	return SBC;
}

Instruction helperFunc2(Byte loBits, Byte hiBits) {
	if (hiBits < 2)
		return ASL;
	if (hiBits < 4)
		return ROL;
	if (hiBits < 6)
		return LSR;
	if (hiBits < 8)
		return ROR;
	if (hiBits < 10)
		return loBits == 14 && hiBits == 9 ? STZ : STA;
	if (hiBits < 12)
		return LDX;
	if (hiBits < 14)
		return DEC;
	return INC;
}

Instruction getInstr(OpCode code) {
	Byte loBits = code & 0x0F;
	Byte hiBits = code >> 4;

	if (loBits == 1 || loBits == 5 || loBits == 9 || loBits == 13) {
		return helperFunc1(loBits, hiBits);
	}
	if (loBits == 6 || loBits == 14) {
		return helperFunc2(loBits, hiBits);
	}
	if (loBits == 0) {
		switch (hiBits) {
		case 0: return BRK;
		case 1: return BPL;
		case 2: return JSR;
		case 3: return BMI;
		case 4: return RTI;
		case 5: return BVC;
		case 6: return RTS;
		case 7: return BVS;
		case 8: return BRA;
		case 9: return BCC;
		case 10: return LDY;
		case 11: return BCS;
		case 12: return CPY;
		case 13: return BNE;
		case 14: return CPX;
		case 15: return BEQ;
		}
	}

	if (loBits == 8) {
		switch (hiBits) {
		case 0: return PHP;
		case 1: return CLC;
		case 2: return PLP;
		case 3: return SEC;
		case 4: return PHA;
		case 5: return CLI;
		case 6: return PLA;
		case 7: return SEI;
		case 8: return DEY;
		case 9: return TYA;
		case 10: return TAY;
		case 11: return CLV;
		case 12: return INY;
		case 13: return CLD;
		case 14: return INX;
		case 15: return SED;
		}
	}

	if (loBits == 2) {
		if (hiBits == 10)
			return LDX;
		return hiBits % 2 == 1 ? helperFunc1(loBits, hiBits) : NOP;
	}

	if (loBits == 3)
		return NOP;

	if (loBits == 7 ||loBits == 15) {  //TODO
		return NOP;
	}

	if (loBits == 10) {
		if (hiBits < 7 && hiBits % 2 == 0)
			return helperFunc2(loBits, hiBits);
		switch (hiBits){
		case 1: return INC;
		case 3: return DEC;
		case 5: return PHY;
		case 7: return PLY;
		case 8: return TXA;
		case 9: return TXS;
		case 10: return TAX;
		case 11: return TSX;
		case 12: return DEX;
		case 13: return PHX;
		case 14: return NOP;
		case 15: return PLX;
		}
	}
	if (loBits == 11) {
		if (hiBits == 12)
			return WAI;
		if (hiBits == 13)
			return STP;
		return NOP;
	}

	if (loBits == 4 || loBits == 12) {
		if (hiBits == 0)
			return TSB;
		if (hiBits == 1)
			return TRB;
		if (hiBits < 4)
			return BIT;
		if (hiBits == 4 && loBits == 12) {
			return JMP;
		}
		if (hiBits == 5)
			return NOP;
		if (hiBits < 8) {
			return loBits == 4 ? STZ : JMP;
		}
		if (hiBits < 10)
			return loBits == 12 && hiBits == 9 ? STZ : STY;
		if (hiBits < 12)
			return LDY;
		if (hiBits == 12)
			return CPY;
		if (hiBits == 14)
			return CPX;
		return NOP;
	}
	return NOP;
}




#endif