#include "Common/FileUtils.h"

#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace FileUtils {
	size_t readFile(const std::string& filename, std::string& fileContent) {
		std::ifstream file(filename, std::ios::ate);
		if (file.is_open()) {
			size_t len = file.tellg();
			file.seekg(0);
			size_t off = fileContent.size();
			fileContent.resize(off + len);
			file.read(fileContent.data() + off, len);
			file.close();
			fileContent.shrink_to_fit();
			return len;
		}
		return 0;
	}

	size_t readFileBinary(const std::string& filename, ByteBuffer& bytes) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (file.is_open()) {
			size_t len = file.tellg();
			file.seekg(0);
			size_t off = bytes.size();
			bytes.resize(off + len);
			file.read(reinterpret_cast<char*>(bytes.data()) + off, len);
			file.close();
			return len;
		}
		return 0;
	}

	void writeFile(const std::string& filename, const std::string& fileContent) {
		std::ofstream file(filename);
		if (file.is_open()) {
			file.write(fileContent.data(), fileContent.size());
			file.close();
		}
	}

	void appendFile(const std::string& filename, const std::string& fileContent) {
		std::ofstream file(filename, std::ios::app);
		if (file.is_open()) {
			file.write(fileContent.data(), fileContent.size());
			file.close();
		}
	}

	void writeFileBinary(const std::string& filename, const ByteBuffer& bytes) {
		std::ofstream file(filename, std::ios::binary);
		if (file.is_open()) {
			file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
			file.close();
		}
	}

	void appendFileBinary(const std::string& filename, const ByteBuffer& bytes) {
		std::ofstream file(filename, std::ios::binary | std::ios::app);
		if (file.is_open()) {
			file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
			file.close();
		}
	}

	void deleteFile(const std::string& filename) {
		std::filesystem::remove(filename);
	}

	void writeLCO(const std::string& filename, const LCO& lco) {
		ByteBuffer outBytes;
		// Header:
		outBytes.addStringNNT("\x45\x54LCROSS_OBJ");   // Magic Numbers
		outBytes.addUInt64(1);                         // LCO Format Version
		outBytes.addUInt32((uint32_t) lco.outputArch); // Output Architecture
		// End Header.
		std::unordered_map<std::string, uint64_t> nameOffsets;
		{
			// Name Table:
			ByteBuffer nameBytes;
			for (const Symbol& sym : lco.symbols) {
				nameOffsets.insert({ sym.name, nameBytes.size() });
				nameBytes.addString(sym.name);
			}

			for (const Section& sect : lco.sections) {
				nameOffsets.insert({ sect.name, nameBytes.size() });
				nameBytes.addString(sect.name);
			}
			outBytes.addUInt64(nameBytes.size()); // Size of Name Table
			outBytes.addBytes(nameBytes);         // Null terminated strings
			                                      // End Name Table.
		}

		// Symbol Table:
		outBytes.addUInt64(lco.symbols.size()); // Num of symbols
		for (const Symbol& sym : lco.symbols) {
			auto nameOffset = nameOffsets.find(sym.name);
			if (nameOffset == nameOffsets.end()) throw std::exception("Symbol name was not found in offsets!");
			outBytes.addUInt64(nameOffset->second);    // Offset of name from start of Name Table
			outBytes.addUInt32((uint32_t) sym.type);   // Type of symbol
			outBytes.addUInt64(sym.sectionIndex);      // Index of section in Section Table where this symbol is located
			outBytes.addUInt64(sym.sectionByteOffset); // The offset from the start of the Section's bytes
			outBytes.addUInt64(sym.sectionByteLength); // The length of data
		}
		// End Symbol Table.

		// Symbol Addr Table:
		outBytes.addUInt64(lco.symbolAddrs.size()); // Num of symbol addrs
		for (const SymbolAddr& symAddr : lco.symbolAddrs) {
			outBytes.addUInt16(symAddr.bits);              // The number of bits requested for getting the address
			outBytes.addUInt64(symAddr.symbolIndex);       // Index of symbol in Symbol Table this symbol addr refers to
			outBytes.addUInt64(symAddr.sectionIndex);      // Index of section in Section Table where this symbol addr is located
			outBytes.addUInt64(symAddr.sectionByteOffset); // The offset where this symbol addr will replace bytes at
		}
		// End Symbol Addr Table.

		// Section Table:
		outBytes.addUInt64(lco.sections.size()); // Num of sections
		uint64_t byteOffset = 0;
		for (const Section& sect : lco.sections) {
			auto nameOffset = nameOffsets.find(sect.name);
			if (nameOffset == nameOffsets.end()) throw std::exception("Section name was not found in offsets!");
			outBytes.addUInt64(nameOffset->second); // Offset of name from start of Name Table
			outBytes.addUInt64(byteOffset);         // Where in Bytes is this section starting
			outBytes.addUInt64(sect.bytes.size());  // Length of this sections bytes
			byteOffset += sect.bytes.size();
		}
		// End Section Table.

		// Bytes:
		{
			ByteBuffer bytes;
			for (const Section& sect : lco.sections)
				bytes.addBytes(sect.bytes);
			outBytes.addUInt64(bytes.size()); // Size of bytes
			outBytes.addBytes(bytes);         // Bytes
		}
		// End Bytes.

		writeFileBinary(filename, outBytes);
	}

	bool readLCO(const std::string& filename, LCO& lco) {
		ByteBuffer inBytes;
		readFileBinary(filename, inBytes);
		lco.outputArch = Arch::DEFAULT;
		lco.symbols.clear();
		lco.symbolAddrs.clear();
		lco.sections.clear();

		// Header:
		std::string magicNumber = inBytes.getStringNNT(12); // Magic number
		if (magicNumber != "\x45\x54LCROSS_OBJ")
			return false;

		uint64_t lcoFormatVersion = inBytes.getUInt64(); // LCO Format Version
		switch (lcoFormatVersion) {
		case 1: {
			lco.outputArch = (Arch) inBytes.getUInt32(); // Output Architecture
			// End Header.

			// Name Table:
			ByteBuffer nameBytes;
			uint64_t nameBytesLength = inBytes.getUInt64(); // Size of Name Table
			inBytes.getBytes(nameBytesLength, nameBytes);   // Null terminated strings
			// End Name Table.

			// Symbol Table:
			uint64_t numSymbols = inBytes.getUInt64(); // Num of symbols
			lco.symbols.resize(numSymbols);
			for (size_t i = 0; i < numSymbols; i++) {
				Symbol& sym           = lco.symbols[i];
				sym.name              = nameBytes.getString(inBytes.getUInt64()); // Offset of name from start of Name Table
				sym.type              = (SymbolType) inBytes.getUInt32();         // Type of symbol
				sym.sectionIndex      = inBytes.getUInt64();                      // Index of section in Section Table where this symbol is located
				sym.sectionByteOffset = inBytes.getUInt64();                      // The offset from the start of the Section's bytes
				sym.sectionByteLength = inBytes.getUInt64();                      // The length of data
			}
			// End Symbol Table.

			// Symbol Addr Table:
			uint64_t numSymbolAddrs = inBytes.getUInt64();
			lco.symbolAddrs.resize(numSymbolAddrs);
			for (size_t i = 0; i < numSymbolAddrs; i++) {
				SymbolAddr& symAddr       = lco.symbolAddrs[i];
				symAddr.bits              = inBytes.getUInt16(); // The number of bits requested for getting the address
				symAddr.symbolIndex       = inBytes.getUInt64(); // Index of symbol in Symbol Table this symbol addr refers to
				symAddr.sectionIndex      = inBytes.getUInt64(); // Index of section in Section Table where this symbol addr is located
				symAddr.sectionByteOffset = inBytes.getUInt64(); // The offset where this symbol addr will replace bytes at
			}
			// End Symbol Addr Table.

			ByteBuffer bytes;
			// Section Table:
			uint64_t numSections = inBytes.getUInt64();
			uint64_t bytesOffset = inBytes.getIndex() + (numSections * 24) + 8;
			uint64_t byteLength  = inBytes.getUInt64(bytesOffset - 8);
			inBytes.getBytes(bytesOffset, byteLength, bytes);
			lco.sections.resize(numSections);
			for (size_t i = 0; i < numSections; i++) {
				Section& sect   = lco.sections[i];
				sect.name       = nameBytes.getString(inBytes.getUInt64()); // Offset of name from start of Name Table
				uint64_t offset = inBytes.getUInt64();                      // Where in Bytes is this section starting
				uint64_t length = inBytes.getUInt64();                      // Length of this sections bytes
				bytes.getBytes(offset, length, sect.bytes);
			}
			// End Section Table.
			return true;
		}
		}
		return false;
	}
} // namespace FileUtils