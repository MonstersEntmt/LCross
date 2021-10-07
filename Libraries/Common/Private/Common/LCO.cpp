#include "LCross/Common/LCO.hpp"
#include "LCross/Common/FileHelper.hpp"

#include <unordered_map>

namespace LCross::Common::LCO {
	namespace V1 {
		void read(ByteBuffer& inBytes, LCO& lco);
	}

	Symbol::Symbol() { }

	Symbol::Symbol(std::string_view name, SymbolType type)
	    : name(name), type(type) { }

	Section::Section() { }

	Section::Section(std::string_view name)
	    : name(name) { }

	Symbol* LCO::getSymbol(std::string_view name) const {
		for (const Symbol& sym : this->symbols)
			if (sym.name == name)
				return const_cast<Symbol*>(&sym);
		return nullptr;
	}

	Section* LCO::getSection(std::string_view name) const {
		for (const Section& sect : this->sections)
			if (sect.name == name)
				return const_cast<Section*>(&sect);
		return nullptr;
	}

	void writeLCO(const std::filesystem::path& filepath, const LCO& lco) {
		ByteBuffer outBytes;
		// Header:
		outBytes.addStringNT("\x45\x54LCROSS_OBJ");                  // Magic Numbers
		outBytes.addUI8(1);                                          // LCO Format Version
		outBytes.addUI4(static_cast<std::uint32_t>(lco.outputArch)); // Output Architecture
		// End Header.
		std::unordered_map<std::string, std::uint64_t> nameOffsets;
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
			outBytes.addUI8(nameBytes.size()); // Size of Name Table
			outBytes.addUI1s(nameBytes);       // Null terminated strings
			                                   // End Name Table.
		}

		// Symbol Table:
		outBytes.addUI8(lco.symbols.size()); // Num of symbols
		for (const Symbol& sym : lco.symbols) {
			auto nameOffset = nameOffsets.find(sym.name);
			if (nameOffset == nameOffsets.end())
				throw std::runtime_error("Symbol name was not found in offsets!");
			outBytes.addUI8(nameOffset->second);                   // Offset of name from start of Name Table
			outBytes.addUI4(static_cast<std::uint32_t>(sym.type)); // Type of symbol
			outBytes.addUI1(static_cast<std::uint8_t>(sym.state)); // State of the symbol
			outBytes.addUI8(sym.sectionIndex);                     // Index of section in Section Table where this symbol is located
			outBytes.addUI8(sym.sectionByteOffset);                // The offset from the start of the Section's bytes
			outBytes.addUI8(sym.sectionByteLength);                // The length of data
			outBytes.addUI8(sym.originAddress);                    // The origin address of the symbol
		}
		// End Symbol Table.

		// Symbol Addr Table:
		outBytes.addUI8(lco.symbolAddrs.size()); // Num of symbol addrs
		for (const SymbolAddr& symAddr : lco.symbolAddrs) {
			outBytes.addUI2(symAddr.bits);              // The number of bits requested for getting the address
			outBytes.addUI8(symAddr.symbolIndex);       // Index of symbol in Symbol Table this symbol addr refers to
			outBytes.addUI8(symAddr.sectionIndex);      // Index of section in Section Table where this symbol addr is located
			outBytes.addUI8(symAddr.sectionByteOffset); // The offset where this symbol addr will replace bytes at
		}
		// End Symbol Addr Table.

		// Section Table:
		outBytes.addUI8(lco.sections.size()); // Num of sections
		uint64_t byteOffset = 0;
		for (const Section& sect : lco.sections) {
			auto nameOffset = nameOffsets.find(sect.name);
			if (nameOffset == nameOffsets.end())
				throw std::runtime_error("Section name was not found in offsets!");
			outBytes.addUI8(nameOffset->second); // Offset of name from start of Name Table
			outBytes.addUI8(byteOffset);         // Where in Bytes is this section starting
			outBytes.addUI8(sect.bytes.size());  // Length of this sections bytes
			byteOffset += sect.bytes.size();
		}
		// End Section Table.

		// Bytes:
		{
			ByteBuffer bytes;
			for (const Section& sect : lco.sections)
				bytes.addUI1s(sect.bytes);
			outBytes.addUI8(bytes.size()); // Size of bytes
			outBytes.addUI1s(bytes);       // Bytes
		}
		// End Bytes.

		writeBinaryFile(filepath, outBytes);
	}

	bool readLCO(const std::filesystem::path& filepath, LCO& lco) {
		ByteBuffer inBytes;
		readBinaryFile(filepath, inBytes);
		lco.outputArch = Architecture::Default;
		lco.symbols.clear();
		lco.symbolAddrs.clear();
		lco.sections.clear();

		// Header:
		std::string_view magicNumber = inBytes.getStringNT(12); // Magic number
		if (magicNumber != "\x45\x54LCROSS_OBJ")
			return false;

		std::uint64_t lcoFormatVersion = inBytes.getUI8(); // LCO Format Version
		switch (lcoFormatVersion) {
		case 1: {
			V1::read(inBytes, lco);
			return true;
		}
		}
		return false;
	}

	namespace V1 {
		void read(ByteBuffer& inBytes, LCO& lco) {
			lco.outputArch = static_cast<Architecture>(inBytes.getUI4()); // Output Architecture
			// End Header.

			// Name Table:
			ByteBuffer nameBytes;
			std::uint64_t nameBytesLength = inBytes.getUI8(); // Size of Name Table
			inBytes.getUI1s(nameBytes, nameBytesLength);      // Null terminated strings
			// End Name Table.

			// Symbol Table:
			std::uint64_t numSymbols = inBytes.getUI8(); // Num of symbols
			lco.symbols.resize(numSymbols);
			for (size_t i = 0; i < numSymbols; i++) {
				Symbol& sym           = lco.symbols[i];
				sym.name              = nameBytes.getString(inBytes.getUI8());      // Offset of name from start of Name Table
				sym.type              = static_cast<SymbolType>(inBytes.getUI4());  // Type of symbol
				sym.state             = static_cast<SymbolState>(inBytes.getUI1()); // State of symbol
				sym.sectionIndex      = inBytes.getUI8();                           // Index of section in Section Table where this symbol is located
				sym.sectionByteOffset = inBytes.getUI8();                           // The offset from the start of the Section's bytes
				sym.sectionByteLength = inBytes.getUI8();                           // The length of data
				sym.originAddress     = inBytes.getUI8();                           // The Origin address of symbol
			}
			// End Symbol Table.

			// Symbol Addr Table:
			std::uint64_t numSymbolAddrs = inBytes.getUI8();
			lco.symbolAddrs.resize(numSymbolAddrs);
			for (size_t i = 0; i < numSymbolAddrs; i++) {
				SymbolAddr& symAddr       = lco.symbolAddrs[i];
				symAddr.bits              = inBytes.getUI2(); // The number of bits requested for getting the address
				symAddr.symbolIndex       = inBytes.getUI8(); // Index of symbol in Symbol Table this symbol addr refers to
				symAddr.sectionIndex      = inBytes.getUI8(); // Index of section in Section Table where this symbol addr is located
				symAddr.sectionByteOffset = inBytes.getUI8(); // The offset where this symbol addr will replace bytes at
			}
			// End Symbol Addr Table.

			ByteBuffer bytes;
			// Section Table:
			uint64_t numSections = inBytes.getUI8();
			uint64_t bytesOffset = inBytes.getOffset() + (numSections * 24) + 8;
			uint64_t byteLength  = inBytes.getUI8(bytesOffset - 8);
			inBytes.getUI1s(bytes, bytesOffset, byteLength);
			lco.sections.resize(numSections);
			for (size_t i = 0; i < numSections; i++) {
				Section& sect   = lco.sections[i];
				sect.name       = nameBytes.getString(inBytes.getUI8()); // Offset of name from start of Name Table
				uint64_t offset = inBytes.getUI8();                      // Where in Bytes is this section starting
				uint64_t length = inBytes.getUI8();                      // Length of this sections bytes
				bytes.getUI1s(sect.bytes, offset, length);
			}
			// End Section Table.
		}
	} // namespace V1
} // namespace LCross::Common::LCO
