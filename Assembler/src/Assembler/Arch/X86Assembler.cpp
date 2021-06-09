#include <Core.h>
#if _TARGETS_X86_
	#include "Assembler/Arch/X86Assembler.h"

	#include <Common/Ini/Ini.h>
	#include <Common/Logger.h>

	#include <algorithm>
	#include <unordered_map>
	#include <vector>

namespace Assembler::X86 {
	struct Opcode {
	public:
		std::vector<uint16_t> bits;
		std::vector<std::string> operands;
		std::vector<std::string> opcode;
	};

	struct Instruction {
	public:
		std::string mnemonic;
		std::string description;
		std::vector<Opcode> opcodes;
	};

	static std::vector<std::string> registers = {
		"AL",
		"AH",
		"AX",
		"EAX",
		"RAX",
		"BL",
		"BH",
		"BX",
		"EBX",
		"RBX",
		"CL",
		"CH",
		"CX",
		"ECX",
		"RCX",
		"DL",
		"DH",
		"DX",
		"EDX",
		"RDX",
		"SP",
		"ESP",
		"RSP",
		"SI",
		"ESI",
		"RSI",
		"DI",
		"EDI",
		"RDI",
		"ES",
		"CS",
		"SS",
		"DS",
		"FS",
		"GS"
	};
	static std::unordered_map<std::string, Instruction> instructions;

	static void tokenizeOpcode(std::string_view str, std::vector<std::string>& tokens) {
		size_t offset = str.find_first_not_of(' ');
		while (offset < str.length()) {
			size_t end = str.find_first_of(' ', offset);
			tokens.push_back(std::string(str.substr(offset, end - offset)));
			if (end < str.length())
				offset = str.find_first_not_of(' ', end);
			else
				offset = end;
		}
	}

	static int64_t readHexaInt(std::string_view str) {
		return static_cast<int64_t>(std::strtoll(str.data(), nullptr, 16));
	}

	static int64_t readDecimalInt(std::string_view str) {
		return static_cast<int64_t>(std::strtoll(str.data(), nullptr, 10));
	}

	static int64_t readOctalInt(std::string_view str) {
		return static_cast<int64_t>(std::strtoll(str.data(), nullptr, 8));
	}

	static int64_t readBinaryInt(std::string_view str) {
		return static_cast<int64_t>(std::strtoll(str.data(), nullptr, 2));
	}

	static int64_t readInt(std::string_view str) {
		if (str[0] == '0' && str.size() > 1)
			return readOctalInt(str.substr(1));
		else if (str[0] == '0' && str[1] == 'x' && str.size() > 2)
			return readHexaInt(str.substr(2));
		else if (str[0] == '0' && str[1] == 'b' && str.size() > 2)
			return readBinaryInt(str.substr(2));

		switch (str[str.size() - 1]) {
		case 'h':
			return readHexaInt(str);
		case 'o':
			return readOctalInt(str);
		case 'b':
			return readBinaryInt(str);
		default:
			return readDecimalInt(str);
		}
	}

	static bool isOctalDigit(char c) {
		return c >= '0' && c <= '7';
	}

	static bool isBinaryDigit(char c) {
		return c >= '0' && c <= '1';
	}

	static bool isHexa(std::string_view str) {
		if (str[0] == '0' && str[1] == 'x' && str.size() > 2) {
			for (size_t i = 2; i < str.size(); i++)
				if (!std::isxdigit(str[i]))
					return false;
			return true;
		}

		if (str[str.size() - 1] == 'h') {
			for (size_t i = 0; i < str.size() - 1; i++)
				if (!std::isxdigit(str[i]))
					return false;
			return true;
		}
		return false;
	}

	static bool isDecimal(std::string_view str) {
		for (size_t i = str[0] == '-' ? 1 : 0; i < str.size(); i++)
			if (!std::isdigit(str[i]))
				return false;
		return true;
	}

	static bool isOctal(std::string_view str) {
		if (str[0] == '0' && str.size() > 1) {
			for (size_t i = 1; i < str.size(); i++)
				if (str[i] < '0' || str[i] > '7')
					return false;
			return true;
		}

		if (str[str.size() - 1] == 'h') {
			for (size_t i = 0; i < str.size() - 1; i++)
				if (str[i] < '0' || str[i] > '7')
					return false;
			return true;
		}
		return false;
	}

	static bool isBinary(std::string_view str) {
		if (str[0] == '0' && str[1] == 'b' && str.size() > 2) {
			for (size_t i = 2; i < str.size(); i++)
				if (str[i] < '0' || str[i] > '1')
					return false;
			return true;
		}

		if (str[str.size() - 1] == 'h') {
			for (size_t i = 0; i < str.size() - 1; i++)
				if (str[i] < '0' || str[i] > '1')
					return false;
			return true;
		}
		return false;
	}

	static bool isInt(std::string_view str) {
		return isHexa(str) || isDecimal(str) || isOctal(str) || isBinary(str);
	}

	static int sameRegister(std::string a, std::string b) {
		for (char& c : b)
			c = std::toupper(c);

		auto itr = std::find(registers.begin(), registers.end(), a);
		if (itr != registers.end())
			return 1 + (a == b);
		return 0;
	}

	static Instruction* findInstruction(std::string instruction) {
		for (char& c : instruction)
			c = std::toupper(c);
		auto itr = instructions.find(instruction);
		if (itr != instructions.end())
			return &itr->second;
		return nullptr;
	}

	static size_t getAddressSize(std::string str) {
		for (char& c : str)
			c = std::toupper(c);
		if (str == "BYTE")
			return 1;
		else if (str == "WORD")
			return 2;
		else if (str == "DWORD")
			return 4;
		else
			return 0;
	}

	static size_t argumentScore(AssemblerState& state, const std::string& opcode, const std::string& instruction) {
		int sameReg = sameRegister(opcode, instruction);
		switch (sameReg) {
		case 0: { // opcode is not a register
			char addressingCode = opcode[0];
			char operandCode    = opcode[1];

			switch (addressingCode) {
			case 'I': {
				if (isInt(instruction)) {
					int64_t immediateVal = readInt(instruction);
					switch (operandCode) {
					case 'b':
						if (immediateVal >= -128LL && immediateVal <= 127LL)
							return 0x7F;
						break;
					case 'w':
						if (immediateVal >= -32768LL && immediateVal <= 32767LL)
							return 0x7F;
						break;
					case 'd':
						if (immediateVal >= -2147483648LL && immediateVal <= 2147483647LL)
							return 0x7F;
						break;
					case 'q':
						return 0x7F;
					}
				}
				break;
			}
			case 'O': {
				switch (operandCode) {
				case 'b': {
					size_t addressBegin = instruction.find_first_of('[');
					if (addressBegin >= instruction.length())
						return 0;
					size_t addressEnd = instruction.find_first_of(']', addressBegin + 1);
					if (addressEnd >= instruction.length())
						return 0;
					size_t addressSize = getAddressSize(instruction.substr(0, addressBegin));
					if (addressSize > 1)
						return 0;

					size_t segmentOverride  = instruction.find_first_of(':', addressBegin + 1);
					bool hasSegmentOverride = segmentOverride < addressEnd;
					if (!hasSegmentOverride)
						segmentOverride = addressBegin + 1;

					std::string address = instruction.substr(segmentOverride, addressEnd - segmentOverride);
					if (isInt(address)) {
						int64_t value = readInt(address);
						if (value >= -128LL && value <= 127LL)
							return 0x7F;
						return 0;
					}

					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [address](const LabelInfo& label) -> bool {
						return label.name == address;
					});
					if (itr != state.labelInfos.end())
						return 0x3F;
					break;
				}
				case 'w': {
					size_t addressBegin = instruction.find_first_of('[');
					if (addressBegin >= instruction.length())
						return 0;
					size_t addressEnd = instruction.find_first_of(']', addressBegin + 1);
					if (addressEnd >= instruction.length())
						return 0;
					size_t addressSize = getAddressSize(instruction.substr(0, addressBegin));
					if (addressSize > 2)
						return 0;

					size_t segmentOverride  = instruction.find_first_of(':', addressBegin + 1);
					bool hasSegmentOverride = segmentOverride < addressEnd;
					if (!hasSegmentOverride)
						segmentOverride = addressBegin + 1;

					std::string address = instruction.substr(segmentOverride, addressEnd - segmentOverride);
					if (isInt(address)) {
						int64_t value = readInt(address);
						if (value >= -32768LL && value <= 32767LL)
							return 0x7F;
						return 0;
					}

					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [address](const LabelInfo& label) -> bool {
						return label.name == address;
					});
					if (itr != state.labelInfos.end())
						return 0x3F;
					break;
				}
				case 'd': {
					size_t addressBegin = instruction.find_first_of('[');
					if (addressBegin >= instruction.length())
						return 0;
					size_t addressEnd = instruction.find_first_of(']', addressBegin + 1);
					if (addressEnd >= instruction.length())
						return 0;
					size_t addressSize = getAddressSize(instruction.substr(0, addressBegin));
					if (addressSize > 4)
						return 0;

					size_t segmentOverride  = instruction.find_first_of(':', addressBegin + 1);
					bool hasSegmentOverride = segmentOverride < addressEnd;
					if (!hasSegmentOverride)
						segmentOverride = addressBegin + 1;

					std::string address = instruction.substr(segmentOverride, addressEnd - segmentOverride);
					if (isInt(address)) {
						int64_t value = readInt(address);
						if (value >= -2147483648LL && value <= 2147483647LL)
							return 0x7F;
						return 0;
					}

					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [address](const LabelInfo& label) -> bool {
						return label.name == address;
					});
					if (itr != state.labelInfos.end())
						return 0x3F;
					break;
				}
				case 'q': {
					size_t addressBegin = instruction.find_first_of('[');
					if (addressBegin >= instruction.length())
						return 0;
					size_t addressEnd = instruction.find_first_of(']', addressBegin + 1);
					if (addressEnd >= instruction.length())
						return 0;
					size_t addressSize = getAddressSize(instruction.substr(0, addressBegin));
					if (addressSize > 8)
						return 0;

					size_t segmentOverride  = instruction.find_first_of(':', addressBegin + 1);
					bool hasSegmentOverride = segmentOverride < addressEnd;
					if (!hasSegmentOverride)
						segmentOverride = addressBegin + 1;

					std::string address = instruction.substr(segmentOverride, addressEnd - segmentOverride);
					if (isInt(address))
						return 0x7F;

					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [address](const LabelInfo& label) -> bool {
						return label.name == address;
					});
					if (itr != state.labelInfos.end())
						return 0x3F;
					break;
				}
				}
				break;
			}
			case 'L': {
				switch (operandCode) {
				case 'n':
					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [instruction](const LabelInfo& label) -> bool {
						return label.name == instruction;
					});
					if (itr != state.labelInfos.end())
						return 0x7F;
					break;
				}
				break;
			}
			case 'S': {
				switch (operandCode) {
				case 's':
					return 0x3F;
				}
				break;
			}
			}

			return 0;
		}
		case 1:
			return 0;
		case 2:
			return 0xFF;
		default:
			return 0;
		}
	}

	static Opcode* findOpcode(AssemblerState& state, Instruction* instruction, const std::vector<std::string>& instructionOpcode, size_t offset) {
		if (!instruction)
			return nullptr;

		if (instruction->mnemonic == "DB") {
			bool failed = false;
			for (size_t i = offset; i < instructionOpcode.size(); i++) {
				int64_t immediateVal = readInt(instructionOpcode[i]);
				if (immediateVal < -128LL || immediateVal > 127LL) {
					failed = true;
					break;
				}
			}
			if (!failed)
				return &instruction->opcodes[0];
		}

		if (instruction->mnemonic == "DW") {
			bool failed = false;
			for (size_t i = offset; i < instructionOpcode.size(); i++) {
				int64_t immediateVal = readInt(instructionOpcode[i]);
				if (immediateVal < -32768LL || immediateVal > 32767LL) {
					failed = true;
					break;
				}
			}
			if (!failed)
				return &instruction->opcodes[0];
		}

		if (instruction->mnemonic == "DD") {
			bool failed = false;
			for (size_t i = offset; i < instructionOpcode.size(); i++) {
				int64_t immediateVal = readInt(instructionOpcode[i]);
				if (immediateVal < -2147483648LL || immediateVal > 2147483647LL) {
					failed = true;
					break;
				}
			}
			if (!failed)
				return &instruction->opcodes[0];
		}

		if (instruction->mnemonic == "DQ") {
			return &instruction->opcodes[0];
		}

		size_t bestScore   = 0;
		Opcode* bestOpcode = nullptr;

		for (auto& opcode : instruction->opcodes) {
			if (opcode.operands.size() != (instructionOpcode.size() - offset))
				continue;

			size_t score = 0;
			for (size_t i = 0; i < opcode.operands.size(); i++) {
				size_t argScore = argumentScore(state, opcode.operands[i], instructionOpcode[i + offset]);

				if (argScore == 0) {
					score = 0;
					break;
				}

				score += argScore;
			}

			if (score != 0 && std::find(opcode.bits.begin(), opcode.bits.end(), state.bits) == opcode.bits.end()) {
				// TODO: Warn user if no other possible instructions was found.
				score = 0;
			}

			if (score > bestScore) {
				bestScore  = score;
				bestOpcode = &opcode;
			}
		}

		return bestOpcode;
	}

	static void addSegmentOverride(AssemblerState& state, std::string_view segmentOverride, ByteBuffer& bytes) {
		if (segmentOverride == "CS") {
			if (state.bits == 16 || state.bits == 32)
				bytes.addUInt8(0x2E, 0);
			else
				Logger::log(LogSeverity::Warn, "{3}:{4} Segment override {0}'CS'{1} is not compatible with the current bits {0}'{2}'{1}", LogColors::Arg, LogColors::Warn, state.bits, state.currentFileName, state.currentLine);
		} else if (segmentOverride == "DS") {
			if (state.bits == 16 || state.bits == 32)
				bytes.addUInt8(0x3E, 0);
			else
				Logger::log(LogSeverity::Warn, "{3}:{4} Segment override {0}'DS'{1} is not compatible with the current bits {0}'{2}'{1}", LogColors::Arg, LogColors::Warn, state.bits, state.currentFileName, state.currentLine);
		} else if (segmentOverride == "ES") {
			if (state.bits == 16 || state.bits == 32)
				bytes.addUInt8(0x26, 0);
			else
				Logger::log(LogSeverity::Warn, "{3}:{4} Segment override {0}'ES'{1} is not compatible with the current bits {0}'{2}'{1}", LogColors::Arg, LogColors::Warn, state.bits, state.currentFileName, state.currentLine);
		} else if (segmentOverride == "SS") {
			if (state.bits == 16 || state.bits == 32)
				bytes.addUInt8(0x36, 0);
			else
				Logger::log(LogSeverity::Warn, "{3}:{4} Segment override {0}'SS'{1} is not compatible with the current bits {0}'{2}'{1}", LogColors::Arg, LogColors::Warn, state.bits, state.currentFileName, state.currentLine);
		} else if (segmentOverride == "FS") {
			if (state.bits == 16 || state.bits == 32 || state.bits == 64)
				bytes.addUInt8(0x64, 0);
			else
				Logger::log(LogSeverity::Warn, "{3}:{4} Segment override {0}'FS'{1} is not compatible with the current bits {0}'{2}'{1}", LogColors::Arg, LogColors::Warn, state.bits, state.currentFileName, state.currentLine);
		} else if (segmentOverride == "GS") {
			if (state.bits == 16 || state.bits == 32 || state.bits == 64)
				bytes.addUInt8(0x65, 0);
			else
				Logger::log(LogSeverity::Warn, "{3}:{4} Segment override {0}'GS'{1} is not compatible with the current bits {0}'{2}'{1}", LogColors::Arg, LogColors::Warn, state.bits, state.currentFileName, state.currentLine);
		} else {
			Logger::log(LogSeverity::Warn, "{4}:{5} Segment override {0}'{2}'{1} is not compatible with the current bits {0}'{3}'{1}", LogColors::Arg, LogColors::Warn, segmentOverride, state.bits, state.currentFileName, state.currentLine);
		}
	}

	AssemblerError setupAssembler() {
		Ini instructionsIni;
		if (!readFileNoexcept("Arch/x86/Instructions.ini", instructionsIni)) {
			Logger::log(LogSeverity::Error, "Instructions file missing, please reinstall or rebuild");
			return AssemblerError::SETUP_CORRUPTED;
		}

		for (auto instructionsItr = instructionsIni.beginSections(), instructionsEnd = instructionsIni.endSections(); instructionsItr != instructionsEnd; instructionsItr++) {
			IniSection* instructionSection = *instructionsItr;
			auto& instructionIni           = instructionSection->getIni();
			Instruction instruction;
			instruction.mnemonic = instructionSection->getName();
			for (auto& opcodeEntry : instructionIni) {
				auto& opcodeArguments = opcodeEntry->getName();
				if (opcodeArguments == "Description") {
					instruction.description = opcodeEntry->getValue();
					continue;
				}

				Opcode opcode;
				std::vector<std::string> operands;
				tokenizeOpcode(opcodeArguments, operands);

				std::string& bits = operands[0];
				size_t offset     = bits.find_first_not_of(' ');
				while (offset < bits.size()) {
					size_t end = bits.find_first_of('/', offset);
					opcode.bits.push_back(static_cast<uint16_t>(std::stoull(bits.data() + offset, nullptr, 10)));
					if (end < bits.size())
						offset = bits.find_last_not_of(' ', end + 1);
					else
						offset = end;
				}

				opcode.operands = std::vector<std::string>(operands.begin() + 1, operands.end());
				tokenizeOpcode(opcodeEntry->getValue(), opcode.opcode);
				instruction.opcodes.push_back(opcode);
			}

			instructions.insert({ instruction.mnemonic, instruction });
		}
		return AssemblerError::GOOD;
	}

	AssemblerError assemble(AssemblerState& state, LineInfo& lineInfo, const std::vector<std::string>& instruction) {
		Instruction* ins = findInstruction(instruction[0]);
		Opcode* opcode   = findOpcode(state, ins, instruction, 1);
		if (!opcode) {
			Logger::log(LogSeverity::Error, "{}:{} {}'{}'{} does not match any instruction", state.currentFileName, state.currentLine, LogColors::Arg, state.currentLineText, LogColors::Error);
			return AssemblerError::INVALID_INSTRUCTION;
		}

		for (auto& op : opcode->opcode) {
			if (op[0] == '%') {
				if (op == "%Db") {
					for (size_t i = 1; i < instruction.size(); i++)
						lineInfo.bytes.addInt8(static_cast<int8_t>(readInt(instruction[i])));
				} else if (op == "%Dw") {
					for (size_t i = 1; i < instruction.size(); i++)
						lineInfo.bytes.addInt16(static_cast<int16_t>(readInt(instruction[i])));
				} else if (op == "%Dd") {
					for (size_t i = 1; i < instruction.size(); i++)
						lineInfo.bytes.addInt32(static_cast<int32_t>(readInt(instruction[i])));
				} else if (op == "%Dq") {
					for (size_t i = 1; i < instruction.size(); i++)
						lineInfo.bytes.addInt64(readInt(instruction[i]));
				} else if (op == "%RESB") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iq")
							break;

					lineInfo.bytes.addBytes(std::vector<uint8_t>(readInt(instruction[1 + offset]), 0));
				} else if (op == "%RESW") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iq")
							break;

					lineInfo.bytes.addBytes(std::vector<uint8_t>(readInt(instruction[1 + offset]) * 2, 0));
				} else if (op == "%RESD") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iq")
							break;

					lineInfo.bytes.addBytes(std::vector<uint8_t>(readInt(instruction[1 + offset]) * 4, 0));
				} else if (op == "%RESQ") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iq")
							break;

					lineInfo.bytes.addBytes(std::vector<uint8_t>(readInt(instruction[1 + offset]) * 8, 0));
				} else if (op == "%BITS") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iw")
							break;

					state.bits = static_cast<uint16_t>(readInt(instruction[1 + offset]));
					if (state.bits > state.maxBits)
						state.maxBits = state.bits;
				} else if (op == "%ORG") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iq")
							break;

					state.originAddress = readInt(instruction[1 + offset]);
				} else if (op == "%GLOBAL") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Ln")
							break;

					const std::string& labelName = instruction[1 + offset];

					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [labelName](const LabelInfo& label) -> bool {
						return label.name == labelName;
					});
					if (itr != state.labelInfos.end()) {
						if (itr->state != SymbolState::HIDDEN && itr->state != SymbolState::GLOBAL) {
							Logger::log(LogSeverity::Warn, "Can't set label {0}'{2}'{1} as {0}'GLOBAL'{1} as it's already {0}'{3}'{1}, skipping", LogColors::Arg, LogColors::Warn, labelName, itr->state);
							continue;
						}
						itr->state = SymbolState::GLOBAL;
					}
				} else if (op == "%EXTERN") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Ln")
							break;

					const std::string& labelName = instruction[1 + offset];

					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [labelName](const LabelInfo& label) -> bool {
						return label.name == labelName;
					});
					if (itr != state.labelInfos.end()) {
						if (itr->state != SymbolState::HIDDEN && itr->state != SymbolState::EXTERNAL) {
							Logger::log(LogSeverity::Warn, "Can't set label {0}'{2}'{1} as {0}'EXTERNAL'{1} as it's already {0}'{3}'{1}, skipping", LogColors::Arg, LogColors::Warn, labelName, itr->state);
							continue;
						}
						itr->state = SymbolState::EXTERNAL;
					}
				} else if (op == "%Ib") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Ib")
							break;

					lineInfo.bytes.addInt8(static_cast<int8_t>(readInt(instruction[1 + offset])));
				} else if (op == "%Iw") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iw")
							break;

					lineInfo.bytes.addInt16(static_cast<int16_t>(readInt(instruction[1 + offset])));
				} else if (op == "%Id") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Id")
							break;

					lineInfo.bytes.addInt32(static_cast<int32_t>(readInt(instruction[1 + offset])));
				} else if (op == "%Iq") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iq")
							break;

					lineInfo.bytes.addInt64(readInt(instruction[1 + offset]));
				} else if (op == "%Ob") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset][0] == 'O')
							break;

					const std::string& specifier = instruction[1 + offset];
					size_t addressBegin          = specifier.find_first_of('[');
					size_t addressEnd            = specifier.find_first_of(']', addressBegin + 1);

					size_t segmentOverride  = specifier.find_first_of(':', addressBegin + 1);
					bool hasSegmentOverride = segmentOverride < addressEnd;
					if (!hasSegmentOverride)
						segmentOverride = addressBegin + 1;

					if (hasSegmentOverride)
						addSegmentOverride(state, specifier.substr(addressBegin + 1, segmentOverride - (addressBegin + 1)), lineInfo.bytes);

					std::string address = specifier.substr(segmentOverride, addressEnd - segmentOverride);
					if (isInt(address)) {
						lineInfo.bytes.addInt8(static_cast<int8_t>(readInt(address)));
					} else {
						auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [address](const LabelInfo& label) -> bool {
							return label.name == address;
						});
						if (itr != state.labelInfos.end()) {
							lineInfo.labelReferences.push_back({ address, lineInfo.bytes.size(), 8 });
							lineInfo.bytes.addInt8(0);
						}
					}
				} else if (op == "%Ow") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset][0] == 'O')
							break;
					const std::string& specifier = instruction[1 + offset];
					size_t addressBegin          = specifier.find_first_of('[');
					size_t addressEnd            = specifier.find_first_of(']', addressBegin + 1);

					size_t segmentOverride  = specifier.find_first_of(':', addressBegin + 1);
					bool hasSegmentOverride = segmentOverride < addressEnd;
					if (!hasSegmentOverride)
						segmentOverride = addressBegin + 1;

					if (hasSegmentOverride)
						addSegmentOverride(state, specifier.substr(addressBegin + 1, segmentOverride - (addressBegin + 1)), lineInfo.bytes);

					std::string address = specifier.substr(segmentOverride, addressEnd - segmentOverride);
					if (isInt(address)) {
						lineInfo.bytes.addInt16(static_cast<int16_t>(readInt(address)));
					} else {
						auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [address](const LabelInfo& label) -> bool {
							return label.name == address;
						});
						if (itr != state.labelInfos.end()) {
							lineInfo.labelReferences.push_back({ address, lineInfo.bytes.size(), 16 });
							lineInfo.bytes.addInt16(0);
						}
					}
				} else if (op == "%Od") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset][0] == 'O')
							break;

					const std::string& specifier = instruction[1 + offset];
					size_t addressBegin          = specifier.find_first_of('[');
					size_t addressEnd            = specifier.find_first_of(']', addressBegin + 1);

					size_t segmentOverride  = specifier.find_first_of(':', addressBegin + 1);
					bool hasSegmentOverride = segmentOverride < addressEnd;
					if (!hasSegmentOverride)
						segmentOverride = addressBegin + 1;

					if (hasSegmentOverride)
						addSegmentOverride(state, specifier.substr(addressBegin + 1, segmentOverride - (addressBegin + 1)), lineInfo.bytes);

					std::string address = specifier.substr(segmentOverride, addressEnd - segmentOverride);
					if (isInt(address)) {
						lineInfo.bytes.addInt32(static_cast<int32_t>(readInt(address)));
					} else {
						auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [address](const LabelInfo& label) -> bool {
							return label.name == address;
						});
						if (itr != state.labelInfos.end()) {
							lineInfo.labelReferences.push_back({ address, lineInfo.bytes.size(), 32 });
							lineInfo.bytes.addInt32(0);
						}
					}
				} else if (op == "%Oq") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset][0] == 'O')
							break;

					const std::string& specifier = instruction[1 + offset];
					size_t addressBegin          = specifier.find_first_of('[');
					size_t addressEnd            = specifier.find_first_of(']', addressBegin + 1);

					size_t segmentOverride  = specifier.find_first_of(':', addressBegin + 1);
					bool hasSegmentOverride = segmentOverride < addressEnd;
					if (!hasSegmentOverride)
						segmentOverride = addressBegin + 1;

					if (hasSegmentOverride)
						addSegmentOverride(state, specifier.substr(addressBegin + 1, segmentOverride - (addressBegin + 1)), lineInfo.bytes);

					std::string address = specifier.substr(segmentOverride, addressEnd - segmentOverride);
					if (isInt(address)) {
						lineInfo.bytes.addInt64(readInt(address));
					} else {
						auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [address](const LabelInfo& label) -> bool {
							return label.name == address;
						});
						if (itr != state.labelInfos.end()) {
							lineInfo.labelReferences.push_back({ address, lineInfo.bytes.size(), 64 });
							lineInfo.bytes.addInt64(0);
						}
					}
				}

				continue;
			}

			char hh = op[0];
			char lh = op[1];

			if (hh >= 'a')
				hh = hh - 'a' + 10;
			else if (hh >= 'A')
				hh = hh - 'A' + 10;
			else
				hh = hh - '0';

			if (lh >= 'a')
				lh = lh - 'a' + 10;
			else if (lh >= 'A')
				lh = lh - 'A' + 10;
			else
				lh = lh - '0';

			lineInfo.bytes.addUInt8(static_cast<uint8_t>(hh << 4 | lh));
		}

		return AssemblerError::GOOD;
	}
} // namespace Assembler::X86
#endif