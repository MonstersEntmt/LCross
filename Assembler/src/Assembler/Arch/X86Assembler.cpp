#include <Core.h>
#if _TARGETS_X86_
	#include "Assembler/Arch/X86Assembler.h"

	#include <Common/Ini/Ini.h>

	#include <algorithm>
	#include <unordered_map>
	#include <vector>

namespace Archs::X86 {
	static struct Opcode {
	public:
		std::vector<std::string> operands;
		std::vector<std::string> opcode;
	};

	static struct Instruction {
	public:
		std::string mnemonic;
		std::string description;
		std::vector<Opcode> opcodes;
	};

	static std::vector<std::string> registers = { "AL", "AH", "AX", "BL", "BH", "BX", "CL", "CH", "CX", "DL", "DH", "DX", "SP", "SI", "DI", "ES", "CS", "SS", "DS" };
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

	static size_t argumentScore(const std::string& opcode, const std::string& instruction) {
		int sameReg = sameRegister(opcode, instruction);
		switch (sameReg) {
		case 0: { // opcode is not a register
			char addressingCode = opcode[0];
			char operandCode    = opcode[1];

			switch (addressingCode) {
			case 'I': {
				int64_t immediateVal = readInt(instruction);
				switch (operandCode) {
				case 'b':
					if (immediateVal >= -128 && immediateVal <= 127)
						return 0x7F;
					break;
				case 'w':
					if (immediateVal >= -32768 && immediateVal <= 32767)
						return 0x7F;
					break;
				case 'd':
					if (immediateVal >= -2147483648 && immediateVal <= 2147483647)
						return 0x7F;
					break;
				}
				break;
			}
			case 'O': {
				switch (operandCode) {
				case 'b':

					break;
				case 'w':
					break;
				case 'd':
					break;
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
		}
	}

	static Opcode* findOpcode(Instruction* instruction, const std::vector<std::string>& instructionOpcode, size_t offset) {
		if (!instruction)
			return nullptr;

		if (instruction->mnemonic == "DB") {
			bool failed = false;
			for (size_t i = offset; i < instructionOpcode.size(); i++) {
				int64_t immediateVal = readInt(instructionOpcode[i]);
				if (immediateVal < -128 || immediateVal > 127) {
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
				if (immediateVal < -32768 || immediateVal > 32767) {
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
				if (immediateVal < -2147483648 || immediateVal > 2147483647) {
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
				size_t argScore = argumentScore(opcode.operands[i], instructionOpcode[i + offset]);

				if (argScore == 0) {
					score = 0;
					break;
				}

				score += argScore;
			}

			if (score > bestScore) {
				bestScore  = score;
				bestOpcode = &opcode;
			}
		}

		return bestOpcode;
	}

	AssemblerError setupAssembler() {
		Ini instructionsIni;
		if (!readFileNoexcept("Arch/x86/Instructions.ini", instructionsIni))
			return AssemblerError::SETUP_CORRUPTED;

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
				tokenizeOpcode(opcodeArguments, opcode.operands);
				tokenizeOpcode(opcodeEntry->getValue(), opcode.opcode);
				instruction.opcodes.push_back(opcode);
			}

			instructions.insert({ instruction.mnemonic, instruction });
		}
		return AssemblerError::GOOD;
	}

	AssemblerError assemble(const AssemblerOptions& options, const std::vector<std::string>& instruction, ByteBuffer& bytes) {
		Instruction* ins = findInstruction(instruction[0]);
		Opcode* opcode   = findOpcode(ins, instruction, 1);
		if (!opcode)
			return AssemblerError::INVALID_INSTRUCTION;

		for (auto& op : opcode->opcode) {
			if (op[0] == '%') {
				if (op == "%Db") {
					for (size_t i = 1; i < instruction.size(); i++) {
						int64_t immediateVal = readInt(instruction[i]);
						if (immediateVal < -128 || immediateVal > 127)
							return AssemblerError::INVALID_INSTRUCTION;

						bytes.addInt8(static_cast<int8_t>(immediateVal));
					}
				} else if (op == "%Dw") {
					for (size_t i = 1; i < instruction.size(); i++) {
						int64_t immediateVal = readInt(instruction[i]);
						if (immediateVal < -32768 || immediateVal > 32767)
							return AssemblerError::INVALID_INSTRUCTION;

						bytes.addInt16(static_cast<int16_t>(immediateVal));
					}
				} else if (op == "%Dd") {
					for (size_t i = 1; i < instruction.size(); i++) {
						int64_t immediateVal = readInt(instruction[i]);
						if (immediateVal < -2147483648 || immediateVal > 2147483647)
							return AssemblerError::INVALID_INSTRUCTION;

						bytes.addInt32(static_cast<int32_t>(immediateVal));
					}
				} else if (op == "%Dq") {
					for (size_t i = 1; i < instruction.size(); i++) {
						int64_t immediateVal = readInt(instruction[i]);
						bytes.addInt64(immediateVal);
					}
				} else if (op == "%Ib") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Ib")
							break;
					if (offset >= opcode->operands.size())
						return AssemblerError::INVALID_INSTRUCTION;

					int64_t immediateVal = readInt(instruction[1 + offset]);
					if (immediateVal < -128 || immediateVal > 127)
						return AssemblerError::INVALID_INSTRUCTION;

					bytes.addInt8(static_cast<int8_t>(immediateVal));
				} else if (op == "%Iw") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Iw")
							break;
					if (offset >= opcode->operands.size())
						return AssemblerError::INVALID_INSTRUCTION;

					int64_t immediateVal = readInt(instruction[1 + offset]);
					if (immediateVal < -32768 || immediateVal > 32767)
						return AssemblerError::INVALID_INSTRUCTION;

					bytes.addInt16(static_cast<int16_t>(immediateVal));
				} else if (op == "%Id") {
					size_t offset = 0;
					for (; offset < opcode->operands.size(); offset++)
						if (opcode->operands[offset] == "Id")
							break;
					if (offset >= opcode->operands.size())
						return AssemblerError::INVALID_INSTRUCTION;

					int64_t immediateVal = readInt(instruction[1 + offset]);
					if (immediateVal < -2147483648 || immediateVal > 2147483648)
						return AssemblerError::INVALID_INSTRUCTION;

					bytes.addInt32(static_cast<int32_t>(immediateVal));
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

			bytes.addUInt8(static_cast<uint8_t>(hh << 4 | lh));
		}

		return AssemblerError::GOOD;
	}
} // namespace Archs::X86
#endif