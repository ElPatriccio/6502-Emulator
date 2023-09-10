#ifndef RAM_H
#define RAM_H
#include"utils.h"

typedef struct {
	Byte *data;
}RAM;

RAM alloc_Ram() {
	RAM ram = { .data = malloc(sizeof(Byte) * 65536) };
	return ram;
}

Byte getByte(RAM* ram, Word address) {
	return ram->data[address];
}

void writeByte(RAM* ram, Word address, Byte data) {
	ram->data[address] = data;
}

Word getWord(RAM* ram, Word address) {
	 return combineLoHiByte(getByte(ram, address), getByte(ram, address + 1));
}

void writeWord(RAM* ram, Word address, Word data) {
	writeByte(ram, address, data & 0x00FF);
	writeByte(ram, ++address, data >> 8);
}

#endif