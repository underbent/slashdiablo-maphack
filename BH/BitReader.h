#pragma once
#include <string>
#include <vector>


typedef unsigned long ulong;

//extern std::size_t const bits_per_byte;

class BitReader {
public:
	std::size_t offset;
	const unsigned char *data;

	BitReader(const unsigned char *data) : data(data), offset(0) {};

	bool readBool();
	unsigned long read(unsigned int numBits);
	unsigned int getBit(unsigned int bitoffset);
	unsigned long getBits(unsigned int numBits);
};
