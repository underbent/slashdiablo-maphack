#include "BitReader.h"


bool BitReader::readBool() {
	return read(1) > 0;
}

unsigned long BitReader::read(unsigned int numBits) {
	unsigned long result = getBits(numBits);
	offset += numBits;
	return result;
}

unsigned int BitReader::getBit(unsigned int bitoffset) {
	unsigned int c = (unsigned int)(data[bitoffset >> 3]);
	unsigned int bitmask = 1 << (bitoffset & 7);
	return ((c & bitmask) != 0) ? 1 : 0;
}

unsigned long BitReader::getBits(unsigned int numBits) {
	unsigned int bits = 0, count = 0;
	for (unsigned int currentbit = offset; currentbit < offset + numBits; currentbit++, count++) {
		bits = bits | (getBit(currentbit) << count);
	}
	return bits;
}
