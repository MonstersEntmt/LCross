#include <Core.h>
#if _TARGETS_BIN_
	#include "Linker/Format/BinFormat.h"

	#include <map>

namespace Linker::Bin {
	LinkerError link(LinkerState& state, ByteBuffer& bytecode) {
		OutputArch outputArch = state.options.inputFiles[0].outputArch;

		std::map<uint64_t, std::map<uint64_t, size_t>> binaryOffsets;
		uint64_t index = 0;
		for (auto& lco : state.options.inputFiles) {
			if (lco.outputArch != outputArch) {
				Logger::log(LogSeverity::Error, "Arch for one of the input files is not the same as the others");
				return LinkerError::INVALID_OUTPUT_ARCH;
			}

			std::map<uint64_t, size_t>& sectionOffsets = binaryOffsets[index++];
			uint64_t sectionIndex                      = 0;
			for (auto& section : lco.sections) {
				sectionOffsets[sectionIndex++] = bytecode.size();
				bytecode.addBytes(section.bytes);
			}
		}

		index = 0;
		for (auto& lco : state.options.inputFiles) {
			std::map<uint64_t, size_t>& sectionOffsets = binaryOffsets[index++];
			for (auto& symbolAddr : lco.symbolAddrs) {
				Symbol* symbol = &lco.symbols[symbolAddr.symbolIndex];
				if (symbol->state == SymbolState::EXTERNAL) {
					size_t count           = 0;
					Symbol* externalSymbol = nullptr;
					for (auto& otherlcos : state.options.inputFiles) {
						auto itr = std::find_if(otherlcos.symbols.begin(), otherlcos.symbols.end(), [symbol](const Symbol& sym) -> bool {
							return sym.state == SymbolState::GLOBAL && sym.name == symbol->name;
						});
						if (itr != otherlcos.symbols.end()) {
							externalSymbol = &*itr;
							count++;
						}
					}

					if (!externalSymbol) {
						Logger::log(LogSeverity::Error, "External symbol {}'{}'{} not found", LogColors::Arg, symbol->name, LogColors::Error);
						return LinkerError::MISSING_EXTERNAL_SYMBOL;
					} else if (count > 1) {
						Logger::log(LogSeverity::Error, "Multiply defined symbols {}'{}'{} found", LogColors::Arg, symbol->name, LogColors::Error);
						return LinkerError::MULTIPLY_DEFINED_SYMBOLS;
					}

					symbol = externalSymbol;
				}

				switch (symbolAddr.bits) {
				case 8:
					bytecode.replaceUInt8(static_cast<uint8_t>(sectionOffsets[symbol->sectionIndex] + symbol->sectionByteOffset + symbol->originAddress), sectionOffsets[symbolAddr.sectionIndex] + symbolAddr.sectionByteOffset);
					break;
				case 16:
					bytecode.replaceUInt16(static_cast<uint16_t>(sectionOffsets[symbol->sectionIndex] + symbol->sectionByteOffset + symbol->originAddress), sectionOffsets[symbolAddr.sectionIndex] + symbolAddr.sectionByteOffset);
					break;
				case 32:
					bytecode.replaceUInt32(static_cast<uint32_t>(sectionOffsets[symbol->sectionIndex] + symbol->sectionByteOffset + symbol->originAddress), sectionOffsets[symbolAddr.sectionIndex] + symbolAddr.sectionByteOffset);
					break;
				case 64:
					bytecode.replaceUInt64(static_cast<uint64_t>(sectionOffsets[symbol->sectionIndex] + symbol->sectionByteOffset + symbol->originAddress), sectionOffsets[symbolAddr.sectionIndex] + symbolAddr.sectionByteOffset);
					break;
				}
			}
		}
		return LinkerError::GOOD;
	}
} // namespace Linker::Bin
#endif