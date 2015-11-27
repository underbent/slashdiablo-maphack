#include "PEStructs.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

namespace PE {
	const uint IMAGE_DIRECTORY_ENTRY_EXPORT = 0;   // Export Directory
	const uint IMAGE_SIZEOF_FILE_HEADER = 20;
	const uint IMAGE_NT_SIGNATURE = 0x00004550;  // PE00
	const uint IMAGE_NUMBEROF_DIRECTORY_ENTRIES = 16;
	const uint SIZEOF_IMAGEDATA_DIRECTORY = 8;
	const uint IMAGE_NT_OPTIONAL_HDR32_MAGIC = 0x10b;
	const uint IMAGE_NT_OPTIONAL_HDR64_MAGIC = 0x20b;
	const uint IMAGE_ROM_OPTIONAL_HDR_MAGIC = 0x107;
	const uint IMAGE_SIZEOF_SHORT_NAME = 8;
	const uint IMAGE_SIZEOF_SECTION_HEADER = 40;


	/*uint IMAGE_FIRST_SECTION(IMAGE_NT_HEADERS ntheader)
	{
	return (uint)(0x3c + 4 + 20 + ntheader.FileHeader.SizeOfOptionalHeader);
	}*/


	struct IMAGE_SECTION_HEADER
	{
		char Name[8];
		uint VirtualSize;
		uint VirtualAddress;
		uint SizeOfRawData;
		uint PointerToRawData;
		uint PointerToRelocations;
		uint PointerToLinenumbers;
		ushort NumberOfRelocations;
		ushort NumberOfLinenumbers;
		uint Characteristics;
	};


	struct IMAGE_DOS_HEADER
	{      // DOS .EXE header
		ushort e_magic;                     // Magic number
		ushort e_cblp;                      // Bytes on last page of file
		ushort e_cp;                        // Pages in file
		ushort e_crlc;                      // Relocations
		ushort e_cparhdr;                   // Size of header in paragraphs
		ushort e_minalloc;                  // Minimum extra paragraphs needed
		ushort e_maxalloc;                  // Maximum extra paragraphs needed
		ushort e_ss;                        // Initial (relative) SS value
		ushort e_sp;                        // Initial SP value
		ushort e_csum;                      // Checksum
		ushort e_ip;                        // Initial IP value
		ushort e_cs;                        // Initial (relative) CS value
		ushort e_lfarlc;                    // File address of relocation table
		ushort e_ovno;                      // Overlay number
		ushort e_res[4];                    // Reserved words
		ushort e_oemid;                     // OEM identifier (for e_oeminfo)
		ushort e_oeminfo;                   // OEM information; e_oemid specific
		ushort e_res2[10];                  // Reserved words
		int64 e_lfanew;						// File address of new exe header
	};

	/// <summary>
	/// Export Format
	/// </summary>
	struct IMAGE_EXPORT_DIRECTORY
	{
		uint Characteristics;
		uint TimeDateStamp;
		ushort MajorVersion;
		ushort MinorVersion;
		uint Name;
		uint Base;
		uint NumberOfFunctions;
		uint NumberOfNames;
		uint AddressOfFunctions;     // RVA from base of image
		uint AddressOfNames;         // RVA from base of image
		uint AddressOfNameOrdinals;  // RVA from base of image
	};

	/// <summary>
	/// Pointer to IMAGE_NT_HEADER is located at 0x3c
	/// </summary>
#pragma pack(push, 1)
	//
	// Directory format.
	//
	struct IMAGE_DATA_DIRECTORY
	{
		uint VirtualAddress;
		uint Size;
	};

	/// <summary>
	/// Pointer to IMAGE_NT_HEADER is located at 0x3c
	/// address = 0x3c
	///
	/// address  : IMAGE_NT_SIGNATURE
	/// address+4: Machine
	/// address+6: NumberOfSections
	/// etc...
	/// address+22: Characteristics
	/// </summary>
	struct IMAGE_FILE_HEADER
	{
		ushort Machine;
		ushort NumberOfSections;
		uint TimeDateStamp;
		uint PointerToSymbolTable;
		uint NumberOfSymbols;
		/// <summary>
		/// Size of the IMAGE_OPTIONAL_HEADER/64
		/// </summary>
		ushort SizeOfOptionalHeader;
		ushort Characteristics;
	};

	//
	// Optional header format.
	//
	struct IMAGE_OPTIONAL_HEADER
	{
		//
		// Standard fields.
		//
		ushort Magic;
		byte MajorLinkerVersion;
		byte MinorLinkerVersion;
		uint SizeOfCode;
		uint SizeOfInitializedData;
		uint SizeOfUninitializedData;
		uint AddressOfEntryPoint;
		uint BaseOfCode;
		uint BaseOfData;
		//
		// NT additional fields.
		//
		uint ImageBase;
		uint SectionAlignment;
		uint FileAlignment;
		ushort MajorOperatingSystemVersion;
		ushort MinorOperatingSystemVersion;
		ushort MajorImageVersion;
		ushort MinorImageVersion;
		ushort MajorSubsystemVersion;
		ushort MinorSubsystemVersion;
		uint Win32VersionValue;
		uint SizeOfImage;
		uint SizeOfHeaders;
		uint CheckSum;
		ushort Subsystem;
		ushort DllCharacteristics;
		uint SizeOfStackReserve;
		uint SizeOfStackCommit;
		uint SizeOfHeapReserve;
		uint SizeOfHeapCommit;
		uint LoaderFlags;
		uint NumberOfRvaAndSizes;
		IMAGE_DATA_DIRECTORY DataDirectory[16];
	};

	struct IMAGE_OPTIONAL_HEADER64
	{
		ushort Magic;
		byte MajorLinkerVersion;
		byte MinorLinkerVersion;
		uint SizeOfCode;
		uint SizeOfInitializedData;
		uint SizeOfUninitializedData;
		uint AddressOfEntryPoint;
		uint BaseOfCode;
		ulong ImageBase;
		uint SectionAlignment;
		uint FileAlignment;
		ushort MajorOperatingSystemVersion;
		ushort MinorOperatingSystemVersion;
		ushort MajorImageVersion;
		ushort MinorImageVersion;
		ushort MajorSubsystemVersion;
		ushort MinorSubsystemVersion;
		uint Win32VersionValue;
		uint SizeOfImage;
		uint SizeOfHeaders;
		uint CheckSum;
		ushort Subsystem;
		ushort DllCharacteristics;
		ulong SizeOfStackReserve;
		ulong SizeOfStackCommit;
		ulong SizeOfHeapReserve;
		ulong SizeOfHeapCommit;
		uint LoaderFlags;
		uint NumberOfRvaAndSizes;
		IMAGE_DATA_DIRECTORY DataDirectory[16];
	};

	struct IMAGE_NT_HEADERS64
	{
		/// <summary>
		/// 0x3c in the file
		/// </summary>
		uint Signature;
		/// <summary>
		/// 0x3c + 4
		/// </summary>
		IMAGE_FILE_HEADER FileHeader;
		/// <summary>
		/// 0x3c + 4 + 20
		/// </summary>
		IMAGE_OPTIONAL_HEADER64 OptionalHeader;
	};


	/// <summary>
	/// Pointer to IMAGE_NT_HEADER is located at 0x3c
	/// </summary>
	struct IMAGE_NT_HEADERS
	{
		/// <summary>
		/// 0x3c in the file
		/// </summary>
		uint Signature;
		/// <summary>
		/// 0x3c + 4
		/// </summary>
		IMAGE_FILE_HEADER FileHeader;
		/// <summary>
		/// 0x3c + 4 + 20
		/// </summary>
		IMAGE_OPTIONAL_HEADER OptionalHeader;
	};
#pragma pack(pop)

	enum MachineType : ushort
	{
		UNKNOWN = 0x0,
		AMD64 = 0x8664,
		IA64 = 0200
	};


	/*template<typename T>
	T read(const char* src, unsigned int position, uint* newPosition){
	auto val = *(T *)&src[position];
	*newPosition = position + sizeof(T);
	return val;
	}*/

	template<class T>
	T read(const char* src, unsigned int position, uint* newPosition){
		auto val = *(T *)&src[position];
		if (newPosition){
			*newPosition = position + sizeof(T);
		}
		return val;
	}

	uint RVAToOffset(uint rva, IMAGE_SECTION_HEADER *sections, uint count)
	{
		// search all sections for the RVA
		for (uint i = 0; i < count; i++)
		{
			if (rva >= sections[i].VirtualAddress && rva < sections[i].VirtualAddress + sections[i].VirtualSize)
			{
				return (uint)(rva - sections[i].VirtualAddress + sections[i].PointerToRawData);
			}
		}

		return 0;
	}

	ulong GetFunctionOffset(std::string dllPath, std::string functionName)
	{
		// see http://www.microsoft.com/whdc/system/platform/firmware/PECOFF.mspx
		// offset to PE header is always at 0x3C
		// PE header starts with "PE\0\0" = 0x50 0x45 0x00 0x00
		// Followed by 2-byte machine type field

		ifstream inFile(dllPath.c_str(), ifstream::in | ifstream::binary);
		std::streamoff size = 0;
		if (inFile.is_open()) {

			char* oData = 0;

			inFile.seekg(0, ios::end);
			size = inFile.tellg();
			inFile.seekg(0, ios::beg);

			oData = new char[size + 1];
			inFile.read(oData, size);
			oData[size] = '\0';

			//IMAGE_DOS_HEADER dosHeader = ReadStruct<IMAGE_DOS_HEADER>(br);
			uint offset = read<uint>(oData, 0x3c, NULL);

			uint cPosition;
			uint pHead = read<uint>(oData, offset, &cPosition);
			if (pHead != PE::IMAGE_NT_SIGNATURE)    // "PE\0\0" little-endian
			{
				// Not a valid portable executable
				return 0;
			}

			//IMAGE_FILE_HEADER fileHeader = IMAGE_FILE_HEADER.FromStream(br);
			IMAGE_FILE_HEADER fileHeader = read<IMAGE_FILE_HEADER>(oData, cPosition, &cPosition);

			ushort machineType = fileHeader.Machine;
			bool is64Bit = false;
			if (machineType != (ushort)UNKNOWN)
			{
				if (machineType == IA64 || machineType == AMD64)
				{
					is64Bit = true;
				}
			}
			else
			{
				return 0;
			}

			IMAGE_DATA_DIRECTORY exportDirectory;
			if (!is64Bit)
			{
				//IMAGE_OPTIONAL_HEADER optionalHeader = IMAGE_OPTIONAL_HEADER.FromStream(br);
				IMAGE_OPTIONAL_HEADER optionalHeader = read<IMAGE_OPTIONAL_HEADER>(oData, cPosition, &cPosition);
				exportDirectory = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			}
			else
			{
				IMAGE_OPTIONAL_HEADER64 optionalHeader = read<IMAGE_OPTIONAL_HEADER64>(oData, cPosition, &cPosition);
				exportDirectory = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			}
			//UInt32 exportPtr = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
			//fs.Seek(exportPtr, SeekOrigin.Begin);
			// should now be at the .export section!
			uint nSec = fileHeader.NumberOfSections;
			IMAGE_SECTION_HEADER *sec = new IMAGE_SECTION_HEADER[nSec];
			for (uint i = 0; i < nSec; i++)
			{
				sec[i] = read<IMAGE_SECTION_HEADER>(oData, cPosition, &cPosition);
			}

			// = optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			uint exportOffset = RVAToOffset(exportDirectory.VirtualAddress, sec, nSec);
			if (exportOffset > 0)
			{
				cPosition = exportOffset;

				IMAGE_EXPORT_DIRECTORY exports = read<IMAGE_EXPORT_DIRECTORY>(oData, cPosition, &cPosition);

				uint nameOffset = RVAToOffset(exports.AddressOfNames, sec, nSec);
				uint funcOffset = RVAToOffset(exports.AddressOfFunctions, sec, nSec);
				uint ordinalOffset = RVAToOffset(exports.AddressOfNameOrdinals, sec, nSec);

				uint length = min(exports.NumberOfFunctions, exports.NumberOfNames);

				vector<string> names;
				vector<uint> addresses;
				vector<uint> ordinals;

				// Read in all named-function names:
				cPosition = nameOffset;
				for (uint i = 0; i < length; i++)
				{
					uint stringLocation = read<uint>(oData, cPosition, &cPosition);
					uint theStringStart = RVAToOffset(stringLocation, sec, nSec);

					cPosition = theStringStart;
					names.push_back(&oData[cPosition]);

					cPosition = nameOffset + (i + 1) * 4;
				}

				// Read in all named-function offsets:
				cPosition = funcOffset;
				for (uint i = 0; i < length; i++)
				{
					addresses.push_back(read<uint>(oData, cPosition, &cPosition));
				}

				// Read in all named-function ordinals
				cPosition = ordinalOffset;
				for (uint i = 0; i < length; i++)
				{
					ordinals.push_back((uint)(read<ushort>(oData, cPosition, &cPosition) + (exports.Base)));

					uint ord = ordinals[i] - exports.Base;
					if (ord >= 0 && ord < length)
					{
						string name = names[i];
						ulong address = addresses[ord];
						if (name.compare(functionName) == 0){
							return address;
						}
						//printf("%s: %d\n", name.c_str(), address);
					}
				}
			}
		}
		return 0;
	}
}