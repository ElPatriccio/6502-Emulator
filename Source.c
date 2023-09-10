#include<stdlib.h>
#include "cpu.h"
#include "ram.h"
#include "unittesting.h"
#include "instructiondecoder.h"

#define RESET_VECTOR_ADDR 0xFFFC
#define TOTAL_CYCLES 7


void initCPU(CPU *cpu, RAM* ram) {
	cpu->PC = getWord(ram, RESET_VECTOR_ADDR);
	cpu->StackPtr = 0xFD;
}

void initMemory(RAM *ram, Word resetVector) {
	writeWord(ram, RESET_VECTOR_ADDR, resetVector);
}

Byte fetchByte(RAM* ram, Word address, Cycles* cycles) {
	(*cycles)--;
	return  getByte(ram, address);
}

Word fetchWord(RAM* ram, Word address, Cycles* cycles) {
	(*cycles) -= 2;
	return getWord(ram, address);
}

void storeByte(RAM* ram, Word address, Byte data, Cycles* cycles) {
	(*cycles)--;
	writeByte(ram, address, data);
}

void storeWord(RAM* ram, Word address, Word data, Cycles* cycles) {
	storeByte(ram, address, data & 0x0F, cycles);
	storeByte(ram, address++, data >> 4, cycles);
}

void doInstr(CPU*, RAM*, Instruction, AddressMode, Cycles*);

signed short run(CPU* cpu, RAM* ram, Cycles* cycles) {
	char buffer[33] = "";
	while (*cycles > 0) {
		OpCode code = fetchByte(ram, cpu->PC++, cycles);
		_itoa(code, buffer, 16);
		printf(" Instruction fetched: %2s\n", buffer);
		doInstr(cpu, ram, getInstr(code), getAddressMode(code), cycles);
	}

	return *cycles;
}

Word getBaseAdrr(CPU* cpu, RAM* ram, Cycles* cycles) {
	return fetchByte(ram, cpu->PC++, cycles) | (fetchByte(ram, cpu->PC++, cycles) << 8);
}

Word getEffectiveAddr(CPU* cpu, RAM* ram, AddressMode mode, Cycles* cycles) {
	switch (mode) {

	case a:
		return getBaseAdrr(cpu, ram, cycles);

	case absIndexIndirectX:
		return fetchWord(ram, getBaseAdrr(cpu, ram, cycles) + cpu->X, cycles);

	case absIndexX:
		Word addr = getBaseAdrr(cpu, ram, cycles);
		if ((addr & 0x00FF) + cpu->X > 0xFF)
			(*cycles)--;
		return addr + cpu->X;

	case absIndexY:
		Word addr1 = getBaseAdrr(cpu, ram, cycles);
		if ((addr1 & 0x00FF) + cpu->Y > 0xFF)
			(*cycles)--;
		return addr1 + cpu->Y;

	case absIndirect:
		return fetchWord(ram, getBaseAdrr(cpu, ram, cycles), cycles);

	case immediate:
		(*cycles)--;
		return cpu->PC++;

	case PCRelative:
		return cpu->PC + fetchByte(ram, cpu->PC++, cycles);

	case stack:
		(*cycles)--;
		return 0x0100 | cpu->StackPtr;

	case zp:
		return fetchByte(ram, cpu->PC++, cycles);

	case zpIndexIndirectX:
		return fetchWord(ram, fetchByte(ram, cpu->PC++, cycles) + cpu->X, cycles);

	case zpIndexX:
		(*cycles)--;
		return fetchByte(ram, cpu->PC++, cycles) + cpu->X;

	case zpIndexY:
		(*cycles)--;
		return fetchByte(ram, cpu->PC++, cycles) + cpu->Y;

	case zpIndirect:
		return fetchWord(ram, fetchByte(ram, cpu->PC++, cycles), cycles);

	case zpIndexIndirectY:
		return fetchWord(ram, fetchByte(ram, cpu->PC++, cycles), cycles) + cpu->Y;
	
	case A:
		return 0x00;

	case implied:
		(*cycles)--;
		return 0x00;
	}
	return 0x00;
}

void putZerFlag(CPU* cpu, Byte data) {
	if (data == 0)
		setStatusBit(cpu, zero);
	else
		clearStatusBit(cpu, zero);
}

void putNegFlag(CPU* cpu, Byte data) {
	if (data >= 128)
		setStatusBit(cpu, negative);
	else
		clearStatusBit(cpu, negative);
}

void putNegAndZerFlag(CPU* cpu, Byte data) {
	putZerFlag(cpu, data);
	putNegFlag(cpu, data);
}

void doInstr(CPU* cpu, RAM* ram, Instruction instr, AddressMode mode, Cycles* cycles) {
	Word effectiveAddress = getEffectiveAddr(cpu, ram, mode, cycles);

	switch (instr) {
	case ADC:
		(void)instr;
		
		volatile Byte result;
		if (mode == immediate)
			result = cpu->A + getByte(ram, effectiveAddress) + STATUS_BIT(carry);
		else
			result = cpu->A + fetchByte(ram, effectiveAddress, cycles) + STATUS_BIT(carry);

		if (cpu->A <= 127 && result > 127 || cpu->A > 127 && result <= 127)
			setStatusBit(cpu, overflow);
		else
			clearStatusBit(cpu, overflow);

		if (result < cpu->A)
			setStatusBit(cpu, carry);
		else
			clearStatusBit(cpu, carry);

		cpu->A = result;
		putNegAndZerFlag(cpu, result);
		break;

	case AND:
		cpu->A &= getByte(ram, effectiveAddress);
		putNegAndZerFlag(cpu, cpu->A);
		break;

	case ASL:
		if (mode == A) {
			if (cpu->A >= 128)
				setStatusBit(cpu, carry);
			else
				clearStatusBit(cpu, carry);
			cpu->A = cpu->A << 1;
		}
		else {
			Byte data = getByte(ram, effectiveAddress);
			if (data >= 128)
				setStatusBit(cpu, carry);
			else
				clearStatusBit(cpu, carry);
			data = data << 1;
			putNegAndZerFlag(cpu, data);
			storeByte(ram, effectiveAddress, data, cycles);
		}
		break;

	case BBR: break;
	case BBS: break;

	case BCC:
		if (!STATUS_BIT(carry))
			cpu->PC = effectiveAddress;
		break;

	case BCS:
		if (STATUS_BIT(carry))
			cpu->PC = effectiveAddress;
		break;

	case BEQ:
		if (STATUS_BIT(zero))
			cpu->PC = effectiveAddress;
		break;

	case BIT:
		if (cpu->A & fetchByte(ram, effectiveAddress, cycles) & 0x40)
			setStatusBit(cpu, overflow);
		else
			clearStatusBit(cpu, overflow);

		putNegAndZerFlag(cpu, cpu->A & getByte(ram, effectiveAddress));
		break;

	case BMI:
		if (STATUS_BIT(negative)) {
			cpu->PC = effectiveAddress;
		}
		break;

	case BNE:
		if (!STATUS_BIT(zero))
			cpu->PC = effectiveAddress;
		break;

	case BPL:
		if (!STATUS_BIT(negative))
			cpu->PC = effectiveAddress;
		break;

	case BRA:
		cpu->PC = effectiveAddress;
		break;

	case BRK: break;

	case BVC:
		if (!STATUS_BIT(overflow)) {
			cpu->PC = effectiveAddress;
			(*cycles)--;
		}

		break;

	case BVS:
		if (STATUS_BIT(overflow)) {
			cpu->PC = effectiveAddress;
			(*cycles)--;
		}
		break;

	case CLC:
		clearStatusBit(cpu, carry);
		break;

	case CLD:
		clearStatusBit(cpu, decimalMode);
		break;

	case CLI:
		clearStatusBit(cpu, irqbDisable);
		break;

	case CLV:
		clearStatusBit(cpu, overflow);
		break;

	case CMP:
		if (cpu->A > fetchByte(ram, effectiveAddress, cycles))
			setStatusBit(cpu, carry);
		putNegAndZerFlag(cpu, cpu->A - getByte(ram, effectiveAddress));
		break;

	case CPX:
		if (cpu->X > fetchByte(ram, effectiveAddress, cycles))
			setStatusBit(cpu, carry);
		putNegAndZerFlag(cpu, cpu->X - getByte(ram, effectiveAddress));
		break;

	case CPY:
		if (cpu->Y > fetchByte(ram, effectiveAddress, cycles))
			setStatusBit(cpu, carry);
		putNegAndZerFlag(cpu, cpu->Y - getByte(ram, effectiveAddress));
		break;

	case DEC:
		if (mode == A) {
			cpu->A--;
			putNegAndZerFlag(cpu, cpu->A);
		}
		else {
			Byte data = fetchByte(ram, effectiveAddress, cycles) - 1;
			storeByte(ram, effectiveAddress, data, cycles);
			putNegAndZerFlag(cpu, data);
		}
		break;

	case DEX:
		cpu->X--;
		putNegAndZerFlag(cpu, cpu->X);
		break;

	case DEY:
		cpu->Y--;
		putNegAndZerFlag(cpu, cpu->Y);
		break;

	case EOR:
		cpu->A = cpu->A ^ fetchByte(ram, effectiveAddress, cycles);
		putNegAndZerFlag(cpu, cpu->A);
		break;

	case INC:
		if (mode == A) {
			cpu->A++;
			putNegAndZerFlag(cpu, cpu->A);
		}
		else {
			Byte data = fetchByte(ram, effectiveAddress, cycles) + 1;
			storeByte(ram, effectiveAddress, data, cycles);
			putNegAndZerFlag(cpu, data);
		}
		break;

	case INX:
		cpu->X++;
		putNegAndZerFlag(cpu, cpu->X);
		break;

	case INY:
		cpu->Y++;
		putNegAndZerFlag(cpu, cpu->Y);
		break;

	case JMP:
		cpu->PC = effectiveAddress;
		break;

	case JSR:

		storeByte(ram, cpu->StackPtr--, (cpu->PC) >> 8, cycles);
		storeByte(ram, cpu->StackPtr--, (cpu->PC) & 0x00FF, cycles);
		(*cycles)--;
		cpu->PC = effectiveAddress;
		break;

	case LDA:
		if (mode == immediate) {
			cpu->A = getByte(ram, effectiveAddress);
		}
		else {
			cpu->A = fetchByte(ram, effectiveAddress, cycles);
			putNegAndZerFlag(cpu, cpu->A);
		}
		break;

	case LDX:
		cpu->X = fetchByte(ram, effectiveAddress, cycles);
		putNegAndZerFlag(cpu, cpu->X);
		break;

	case LDY:
		cpu->Y = fetchByte(ram, effectiveAddress, cycles);
		putNegAndZerFlag(cpu, cpu->Y);
		break;

	case LSR:
		if (mode == A) {
			if (cpu->A % 2 == 1)
				setStatusBit(cpu, carry);
			else
				clearStatusBit(cpu, carry);

			cpu->A = cpu->A >> 1;
			putNegAndZerFlag(cpu, cpu->A);
		}
		else {
			Byte data = fetchByte(ram, effectiveAddress, cycles);
			if (data % 2 == 1)
				setStatusBit(cpu, carry);
			else
				clearStatusBit(cpu, carry);
			data = data >> 1;
			storeByte(ram, effectiveAddress, data, cycles);
			putNegAndZerFlag(cpu, data);
		}
		break;
	case NOP:
		(*cycles) -= 2;
		break;

	case ORA:
		cpu->A |= fetchByte(ram, effectiveAddress, cycles);
		putNegAndZerFlag(cpu, cpu->A);
		break;

	case PHA:
		storeByte(ram, effectiveAddress, cpu->A, cycles);
		cpu->StackPtr--;
		break;

	case PHP:
		storeByte(ram, effectiveAddress, cpu->ProcStatus, cycles);
		cpu->StackPtr--;
		break;

	case PHX:
		storeByte(ram, effectiveAddress, cpu->X, cycles);
		cpu->StackPtr--;
		break;

	case PHY:
		storeByte(ram, effectiveAddress, cpu->Y, cycles);
		cpu->StackPtr--;
		break;

	case PLA:
		cpu->A = fetchByte(ram, ++effectiveAddress, cycles);
		cpu->StackPtr++;
		putNegAndZerFlag(cpu, cpu->A);
		(*cycles)--;
		break;

	case PLP:
		cpu->ProcStatus = fetchByte(ram, ++effectiveAddress, cycles);
		cpu->StackPtr++;
		(*cycles)--;
		break;

	case PLX:
		cpu->X = fetchByte(ram, ++effectiveAddress, cycles);
		cpu->StackPtr++;
		putNegAndZerFlag(cpu, cpu->X);
		(*cycles)--;
		break;

	case PLY:
		cpu->Y = fetchByte(ram, ++effectiveAddress, cycles);
		cpu->StackPtr++;
		putNegAndZerFlag(cpu, cpu->Y);
		(*cycles)--;
		break;

	case RMB: break;

	case ROL:
		if (mode == A) {
			Byte copy = cpu->A;
			cpu->A = (cpu->A << 1) + STATUS_BIT(carry);
			if (copy > 128)
				setStatusBit(cpu, carry);
			else
				clearStatusBit(cpu, carry);
			putNegAndZerFlag(cpu, cpu->A);
		}
		else {
			Byte data = fetchByte(ram, effectiveAddress, cycles);
			Byte dataCopy = data;
			data = (data << 1) + STATUS_BIT(carry);
			if (dataCopy > 128)
				setStatusBit(cpu, carry);
			else
				clearStatusBit(cpu, carry);
			storeByte(ram, effectiveAddress, data, cycles);
			putNegAndZerFlag(cpu, data);
		}
		break;

	case ROR:
		if (mode == A) {
			Byte copy = cpu->A;
			cpu->A = (cpu->A >> 1) | (STATUS_BIT(carry) << 7);
			if (copy % 2 == 1)
				setStatusBit(cpu, carry);
			else
				clearStatusBit(cpu, carry);
			putNegAndZerFlag(cpu, cpu->A);
		}
		else {
			Byte data = fetchByte(ram, effectiveAddress, cycles);
			Byte dataCopy = data;
			data = data >> 1 | (STATUS_BIT(carry) << 7);
			if (data % 2 == 1)
				setStatusBit(cpu, carry);
			else
				clearStatusBit(cpu, carry);

			storeByte(ram, effectiveAddress, data, cycles);
			putNegAndZerFlag(cpu, data);
			break;
		}

	case RTI: break;
	case RTS:
		cpu->PC = fetchWord(ram, ++cpu->StackPtr, cycles);
		cpu->StackPtr++;
		(*cycles) -= 3;
		break;

	case SBC:
		if (cpu->A > fetchByte(ram, effectiveAddress, cycles))
			setStatusBit(cpu, carry);
		else
			clearStatusBit(cpu, carry);
		Byte operand = getByte(ram, effectiveAddress);
		volatile Byte result1 = cpu->A - operand - !STATUS_BIT(carry);
		if ((cpu->A <= 127 && operand > 127 && result1 > 127) || cpu->A > 128 && operand <= 127 && result1 <= 127)
			setStatusBit(cpu, overflow);
		else
			clearStatusBit(cpu, overflow);
		cpu->A = result1;
		putNegAndZerFlag(cpu, cpu->A);
		break;

	case SEC:
		setStatusBit(cpu, carry);
		break;

	case SED:
		setStatusBit(cpu, decimalMode);
		break;

	case SEI:
		setStatusBit(cpu, irqbDisable);
		break;

	case SMB: break;

	case STA:
		storeByte(ram, effectiveAddress, cpu->A, cycles);
		break;

	case STP: break;

	case STX:
		storeByte(ram, effectiveAddress, cpu->X, cycles);
		break;

	case STY:
		storeByte(ram, effectiveAddress, cpu->Y, cycles);
		break;

	case STZ:
		storeByte(ram, effectiveAddress, 0x00, cycles);
		break;

	case TAX:
		cpu->X = cpu->A;
		putNegAndZerFlag(cpu, cpu->X);
		break;

	case TAY:
		cpu->Y = cpu->A;
		putNegAndZerFlag(cpu, cpu->Y);
		break;

	case TRB:
		storeByte(ram, effectiveAddress, ~cpu->A & fetchByte(ram, effectiveAddress, cycles), cycles);
		putZerFlag(cpu, ~cpu->A & getByte(ram, effectiveAddress));
		break;

	case TSB:
		storeByte(ram, effectiveAddress, cpu->A | fetchByte(ram, effectiveAddress, cycles), cycles);
		putZerFlag(cpu, cpu->A | getByte(ram, effectiveAddress));
		break;

	case TSX:
		cpu->X = cpu->StackPtr;
		putNegAndZerFlag(cpu, cpu->X);
		break;

	case TXA:
		cpu->A = cpu->X;
		putNegAndZerFlag(cpu, cpu->A);
		break;
	case TXS:
		cpu->StackPtr = cpu->X;
		break;

	case TYA:
		cpu->A = cpu->Y;
		putNegAndZerFlag(cpu, cpu->A);
		break;

	case WAI: break;
		
	}
}

int main(int argc, char argv[]) {
	CPU cpu;
	RAM ram = alloc_Ram();
	initMemory(&ram, 0x8000);
	initCPU(&cpu, &ram);
	
	writeByte(&ram, 0x8000, 0xA9);		// LDA => 99
	writeByte(&ram, 0x8001, 0x99);
	writeByte(&ram, 0x8002, 0x48);		// PHA zum Stack
	writeByte(&ram, 0x8003, 0xAE);		// LDX => 0x1
	writeWord(&ram, 0x8004, 0xFFDD);
	writeByte(&ram, 0x8006, 0x1D);		// A = 0xdd
	writeWord(&ram, 0x8007, 0xFFAA);	// FFAB 
	writeByte(&ram, 0x8009, 0xAA);		// X = 0xdd
	writeByte(&ram, 0x800A, 0x68);		// A = (0xFC+1) (0x99)

	writeByte(&ram, 0xFFAB, 0x44);
	writeByte(&ram, 0xFFDD, 0x01);   
																        
	                               

	Cycles cyclesToDo = 19;
	Cycles cyclesToDoCopy = cyclesToDo;

	if (run(&cpu, &ram, &cyclesToDo) < 0)
		printf("\n Invalid Cycles provided!\n Cycles used: %d\tCycles provided: %d\n", cyclesToDoCopy - cyclesToDo, cyclesToDoCopy);
	else
		printf(" Finished! Cycles used: %d (including reset routine: %d)\n", cyclesToDoCopy,  TOTAL_CYCLES + cyclesToDoCopy);
	
	printInfo(&cpu);

	free(ram.data);
	return 0;
}