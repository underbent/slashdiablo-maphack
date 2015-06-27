#pragma once
#include <map>

namespace PE {

	/* Author: slayergod13
	*  Taken from some C# code of mine, hence the type names
	*/
	typedef unsigned int uint;
	typedef unsigned short ushort;
	typedef unsigned char byte;
	typedef unsigned long long ulong;
	typedef long long int64;




	ulong GetFunctionOffset(std::string dllPath, std::string functionName);
}