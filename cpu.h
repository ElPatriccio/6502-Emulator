#ifndef CPU_H
#define CPU_H

#include<stdio.h>
#include"utils.h"



typedef struct {
	Byte A;
	Byte X;
	Byte Y;
	Byte StackPtr;
	Byte ProcStatus;
	Word PC;

}CPU;

Byte getStatusBit(const CPU* cpu, StatusBit bit) {
	return (cpu->ProcStatus & bit) > 0;
}

void setStatusBit(CPU* cpu, StatusBit bit) {
	cpu->ProcStatus |= bit;
}

void clearStatusBit(CPU* cpu, StatusBit bit) {
	cpu->ProcStatus &= ~bit;
}



void printInfo(const CPU* cpu) {
	char buf[33] = "";
	_itoa(cpu->A, buf, 16);
	printf(" -------------------\n PROCESSOR INTERNALS\n -------------------\n REGISTERS:\n   A: %5s\n", buf);
	_itoa(cpu->X, buf, 16);
	printf("   X: %5s\n", buf);
	_itoa(cpu->Y, buf, 16);
	printf("   Y: %5s\n", buf);
	_itoa(cpu->StackPtr, buf, 16);
	printf("  SP: %5s\n", buf);
	_itoa(cpu->PC, buf, 16);
	printf("  PC: %5s\n\n", buf);
	printf(" STATUS:\n   N: %d\n   V: %d\n   x: %d\n   B: %d\n   D: %d\n   I: %d\n   Z: %d\n   C: %d\n", getStatusBit(cpu, negative), getStatusBit(cpu, overflow), getStatusBit(cpu, custom),
		getStatusBit(cpu, brkCommand), getStatusBit(cpu, decimalMode), getStatusBit(cpu, irqbDisable), getStatusBit(cpu, zero),
		getStatusBit(cpu, carry));
}

#endif
