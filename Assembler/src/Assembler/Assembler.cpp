#include "Assembler/Assembler.h"

#include <Common/Logger.h>
#include <Common/PrintUtils.h>
#include <Core.h>

#if _TARGETS_X86_
	#include "Assembler/Arch/X86Assembler.h"
#endif
#if _TARGETS_ARM32_
	#include "Assembler/Arch/Arm32Assembler.h"
#endif
#if _TARGETS_ARM64_
	#include "Assembler/Arch/Arm64Assembler.h"
#endif

AssemblerArgUtils::AssemblerArgUtils(int argc, char** argv)
#ifdef _NO_LINKER_
    : ArgUtils(argc, argv) {
#else
    : LinkerArgUtils(argc, argv) {
#endif
	registerFlagInfo(FlagInfo("-a", "Select output architecture", "", { FlagValueInfo("architecture", FlagValueType::REQUIRED, {
	                                                                                                                               "default",
#ifdef _TARGETS_X86_
	                                                                                                                               "x86",
#endif
#ifdef _TARGETS_X86_64_
	                                                                                                                               "x86_64",
#endif
#ifdef _TARGETS_ARM32_
	                                                                                                                               "arm32",
#endif
#ifdef _TARGETS_ARM64_
	                                                                                                                               "arm64",
#endif
	                                                                                                                           }) }));
	registerHandlerFunc("-a", std::bind(&AssemblerArgUtils::handleArchFlag, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
#ifndef _NO_LINKER_
	registerFlagInfo(FlagInfo("-no_link", "Forces no linkage of file inputs", "This causes '<input filenames ...>' to be '<input filename>', though it will not give errors due to passing too many input filenames", {}));
	registerHandlerFunc("-no_link", std::bind(&AssemblerArgUtils::handleNoLinkFlag, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
#endif
}

void AssemblerArgUtils::handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
#ifdef _NO_LINKER_
	ArgUtils::handleVersionFlag(argUtils, arg, argValues, usedValueCount, argFailed);
#else
	LinkerArgUtils::handleVersionFlag(argUtils, arg, argValues, usedValueCount, argFailed);
#endif
	std::vector<std::string> supportedOutputArchitectures {
#ifdef _TARGETS_X86_
		"x86",
#endif
#ifdef _TARGETS_X86_64_
		"x86_64",
#endif
#ifdef _TARGETS_ARM32_
		"arm32",
#endif
#ifdef _TARGETS_ARM64_
		"arm64",
#endif
	};

	std::string supportedArchs;
	size_t numChars = 0;
	for (size_t i = 0; i < supportedOutputArchitectures.size(); i++)
		numChars += supportedOutputArchitectures[i].length();

	if (numChars > 50) {
		std::string str;
		for (std::string& arch : supportedOutputArchitectures)
			str += "\n    '" + arch + "'";
		supportedArchs = Format::format("{}{}{}", LogColors::Arg, str, LogColors::Info);
	} else {
		for (size_t i = 0; i < supportedOutputArchitectures.size(); i++) {
			if (i == 0)
				supportedArchs += " ";
			else
				supportedArchs += ", ";
			supportedArchs += Format::format("{}'{}'{}", LogColors::Arg, supportedOutputArchitectures[i], LogColors::Info);
		}
	}

	Logger::print("{}This build supports these output architectures:{}", LogColors::Info, supportedArchs);

#ifndef _NO_LINKER_
	Logger::print("{}This build has a built in Linker", LogColors::Info);
#endif
}

void AssemblerArgUtils::handleArchFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	std::string argValue = argValues[0];
	if (argValue == "default") this->outputArch = OutputArch::DEFAULT;
#if _TARGETS_X86_64_
	else if (argValue == "x86")
		this->outputArch = OutputArch::X86;
#endif
#if _TARGETS_X86_64_
	else if (argValue == "x86_64")
		this->outputArch = OutputArch::X86_64;
#endif
#if _TARGETS_ARM32_
	else if (argValue == "arm32")
		this->outputArch = OutputArch::ARM32;
#endif
#if _TARGETS_ARM64_
	else if (argValue == "arm64")
		this->outputArch = OutputArch::ARM64;
#endif
	usedValueCount = 1;
}

#ifndef _NO_LINKER_
void AssemblerArgUtils::handleNoLinkFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	this->noLink = true;
}
#endif

std::string AssemblerArgUtils::getFullUsageString(const FlagInfo* currentFlag) {
#ifndef _NO_LINKER_
	if (currentFlag) {
		if (currentFlag->name == "-h" || currentFlag->name == "-v") {
			return currentFlag->getSyntax();
		} else {
			if (currentFlag->name == "-no_link") {
				return "{flags {value} ...} -no_link <input filename>";
			} else {
				if (this->noLink) {
					return "{flags {value} ...} " + currentFlag->getSyntax() + " -no_link <input filename>";
				} else {
					return "{flags {value} ...} " + currentFlag->getSyntax() + " <input filenames ...>";
				}
			}
		}
	} else {
		if (this->noLink) {
			return "{flags {value} ...} -no_link <input filename>";
		} else {
			return "{flags {value} ...} <input filenames ...>";
		}
	}
#else
	if (currentFlag) {
		if (currentFlag->name == "-h" || currentFlag->name == "-v") {
			return currentFlag->getSyntax();
		} else {
			return "{flags {value} ...} " + currentFlag->getSyntax() + " <input filename>";
		}
	} else {
		return "{flags {value} ...} <input filename>";
	}
#endif
}

void AssemblerArgUtils::handleOutputName() {
#ifndef _NO_LINKER_
	if (this->noLink) {
#endif
		this->outputName = this->outputName.substr(0, this->outputName.find_last_of('.'));
		this->outputName += ".lco";
#ifndef _NO_LINKER_
	} else {
		LinkerArgUtils::handleOutputName();
	}
#endif
}

void AssemblerArgUtils::handleVirt() {
#ifndef _NO_LINKER_
	LinkerArgUtils::handleVirt();
#endif
	if (this->outputArch == OutputArch::DEFAULT)
		this->outputArch = getDefaultCompileArchForHost();
}

std::ostream& operator<<(std::ostream& ostream, AssemblerError error) {
	switch (error) {
	case AssemblerError::GOOD: return ostream << "Good";
	case AssemblerError::NOT_IMPLEMENTED: return ostream << "Assembler not implemented";
	case AssemblerError::INVALID_OUTPUT_ARCH: return ostream << "Invalid Output Architecture";
	case AssemblerError::INVALID_INSTRUCTION: return ostream << "Invalid Instruction";
	case AssemblerError::INVALID_LABEL: return ostream << "Invalid Label";
	case AssemblerError::MULTIPLY_DEFINED_LABELS: return ostream << "Multiply Defined Labels";
	case AssemblerError::SETUP_CORRUPTED: return ostream << "Setup Corrupted";
	default: return ostream << "Unknown";
	}
}

namespace Assembler {
	using AssemblerFunction = std::function<AssemblerError(AssemblerState& state, LineInfo& line, const std::vector<std::string>& instruction)>;

	void tokenize(const std::string& str, std::vector<std::vector<std::string>>& tokens) {
		std::string_view strvw = str;
		std::vector<std::string_view> lines;
		{
			size_t offset = str.find_first_not_of('\n');
			while (offset < str.length()) {
				size_t end = str.find_first_of('\n', offset);
				lines.push_back(strvw.substr(offset, end - offset));
				if (end < str.length())
					offset = end + 1;
				else
					offset = end;
			}
		}

		for (size_t i = 0; i < lines.size(); i++) {
			tokens.push_back({});
			std::vector<std::string>& tokenLine = tokens[tokens.size() - 1];

			std::string_view line = lines[i];
			tokenLine.push_back(std::string(line));
			if (line.length() == 0)
				continue;

			size_t commentBegin = line.find_first_of(';');
			if (commentBegin < line.length())
				line = line.substr(0, commentBegin);

			size_t offset   = line.find_first_not_of(' ');
			size_t labelEnd = line.find_first_of(':');
			if (labelEnd < line.length()) {
				tokenLine.push_back(std::string(line.substr(offset, labelEnd)));
				tokenLine.push_back(":");
				offset = line.find_first_not_of(' ', labelEnd + 1);
			}
			if (offset >= line.length())
				continue;

			size_t instructionEnd        = line.find_first_of(' ', offset);
			std::string_view instruction = line.substr(offset, instructionEnd - offset);
			if (instruction.length() > 0) {
				tokenLine.push_back(std::string(instruction));
				offset = line.find_first_not_of(' ', instructionEnd);
			}

			while (offset < line.length()) {
				size_t pe         = line.find_first_of(',', offset);
				size_t tokenStart = line.find_first_not_of(' ', offset);
				size_t tokenEnd   = line.find_last_not_of(' ', pe - 1);
				tokenLine.push_back(std::string(line.substr(tokenStart, tokenEnd - tokenStart + 1)));
				if (pe < line.length())
					offset = line.find_first_not_of(' ', pe + 1);
				else
					offset = pe;
			}
		}
	}

	AssemblerError assemble(AssemblerState& state, const std::string& fileContent, LCO& lco) {
		AssemblerFunction assemblerFunc = nullptr;
		switch (state.options.outputArch) {
#if _TARGETS_X86_
		case OutputArch::X86: {
			AssemblerError error = Assembler::X86::setupAssembler();
			if (error != AssemblerError::GOOD)
				return error;
			assemblerFunc = &Assembler::X86::assemble;
			break;
		}
#endif
#if _TARGETS_X86_64_
		case OutputArch::X86_64: {
			AssemblerError error = Assembler::X86::setupAssembler();
			if (error != AssemblerError::GOOD)
				return error;
			assemblerFunc = &Assembler::X86::assemble;
			state.maxBits = state.bits = 64;
			break;
		}
#endif
#if _TARGETS_ARM32_
		case OutputArch::ARM32:
			assemblerFunc = &Assembler::Arm32::assemble;
			break;
#endif
#if _TARGETS_ARM64_
		case OutputArch::ARM64:
			assemblerFunc = &Assembler::Arm64::assemble;
			state.maxBits = state.bits = 64;
			break;
#endif
		default:
			Logger::log(LogSeverity::Error, "Arch {}'{}'{} not supported", LogColors::Arg, state.options.outputArch, LogColors::Error);
			return AssemblerError::INVALID_OUTPUT_ARCH;
		}

		std::vector<std::vector<std::string>> lines;
		tokenize(fileContent, lines);

		std::string pLabelName;
		for (size_t i = 0; i < lines.size(); i++, state.currentLine++) {
			std::vector<std::string>& tokens = lines[i];
			state.currentLineText            = tokens[0];
			if (tokens.size() >= 3) {
				std::string& first  = tokens[1];
				std::string& second = tokens[2];
				if (first == "#line") {
					if (tokens.size() >= 4) {
						state.currentLine     = strtoull(second.data(), nullptr, 10);
						state.currentFileName = tokens[3];
					} else {
						state.currentLine = strtoull(second.data(), nullptr, 10);
					}
				} else if (second == ":") {
					std::string ln = first;
					if (ln[0] == '.')
						ln = pLabelName + ln;

					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [ln](const LabelInfo& label) -> bool {
						return label.name == ln;
					});
					if (itr != state.labelInfos.end()) {
						Logger::log(LogSeverity::Error, "Label {}'{}'{} already defined at {}:{}", LogColors::Arg, ln, LogColors::Error, itr->filename, itr->fileLineNumber);
						return AssemblerError::MULTIPLY_DEFINED_LABELS;
					}

					state.labelInfos.push_back({ ln, i, state.currentFileName, state.currentLine });
					pLabelName = ln;
				}
			}
		}

		state.currentLine = 0;
		state.lineInfos.resize(lines.size());
		pLabelName.clear();
		for (size_t i = 0; i < lines.size(); i++, state.currentLine++) {
			std::vector<std::string>& tokens = lines[i];
			state.currentLineText            = tokens[0];
			size_t currentToken              = 1;
			if (tokens.size() >= 3) {
				std::string& first  = tokens[1];
				std::string& second = tokens[2];
				if (first == "#line") {
					if (tokens.size() >= 4) {
						state.currentLine     = strtoull(second.data(), nullptr, 10);
						state.currentFileName = tokens[3];
					} else {
						state.currentLine = strtoull(second.data(), nullptr, 10);
					}
					continue;
				} else if (second == ":") {
					std::string ln = first;
					if (ln[0] == '.')
						ln = pLabelName + ln;
					auto itr = std::find_if(state.labelInfos.begin(), state.labelInfos.end(), [ln](const LabelInfo& label) -> bool {
						return label.name == ln;
					});
					if (itr != state.labelInfos.end())
						itr->originAddress = state.originAddress;
					pLabelName = ln;
					currentToken += 2;
				}
			}

			if (currentToken < tokens.size()) {
				std::vector<std::string> instruction(tokens.begin() + currentToken, tokens.end());
				LineInfo& lineInfo      = state.lineInfos[i];
				lineInfo.filename       = state.currentFileName;
				lineInfo.fileLineNumber = state.currentLine;
				lineInfo.sectionName    = state.currentSectionName;
				AssemblerError error    = assemblerFunc(state, lineInfo, instruction);
				if (error != AssemblerError::GOOD)
					return error;
			} else {
				LineInfo& lineInfo      = state.lineInfos[i];
				lineInfo.filename       = state.currentFileName;
				lineInfo.fileLineNumber = state.currentLine;
				lineInfo.sectionName    = state.currentSectionName;
			}
		}

		lco.outputArch = state.options.outputArch;
		for (LineInfo& lineInfo : state.lineInfos) {
			Section* section = lco.getSection(lineInfo.sectionName);
			if (!section) {
				lco.sections.push_back(Section(lineInfo.sectionName));
				section = lco.getSection(lineInfo.sectionName);
			}
			lineInfo.sectionOffset = section->bytes.size();
			section->bytes.addBytes(lineInfo.bytes);
		}

		for (LabelInfo& labelInfo : state.labelInfos) {
			Symbol labelSymbol(labelInfo.name, SymbolType::LABEL);
			labelSymbol.state         = labelInfo.state;
			labelSymbol.originAddress = labelInfo.originAddress;
			if (labelInfo.lineNumber >= state.lineInfos.size()) {
				Logger::log(LogSeverity::Error, "Label {}'{}'{} defined at {}:{} is corrupted, please re-assemble", LogColors::Arg, labelInfo.name, LogColors::Error, labelInfo.filename, labelInfo.fileLineNumber);
				return AssemblerError::INVALID_LABEL;
			}

			const std::string& sectionName = state.lineInfos[labelInfo.lineNumber].sectionName;
			labelSymbol.sectionIndex       = 0;
			for (; labelSymbol.sectionIndex <= lco.sections.size(); labelSymbol.sectionIndex++) {
				if (labelSymbol.sectionIndex == lco.sections.size()) {
					Logger::log(LogSeverity::Error, "Label {}'{}'{} defined at {}:{} is corrupted, please re-assemble", LogColors::Arg, labelInfo.name, LogColors::Error, labelInfo.filename, labelInfo.fileLineNumber);
					return AssemblerError::INVALID_LABEL;
				}

				if (lco.sections[labelSymbol.sectionIndex].name == sectionName)
					break;
			}

			labelSymbol.sectionByteOffset = 0; // Calculate from the assembled binary.
			for (size_t i = 0; i < labelInfo.lineNumber; i++)
				if (state.lineInfos[i].sectionName == sectionName)
					labelSymbol.sectionByteOffset += state.lineInfos[i].bytes.size();

			labelSymbol.sectionByteLength = 0;
			lco.symbols.push_back(labelSymbol);
		}

		for (LineInfo& lineInfo : state.lineInfos) {
			for (LabelReference& labelReference : lineInfo.labelReferences) {
				SymbolAddr addr;
				addr.bits              = labelReference.bits;
				addr.sectionByteOffset = lineInfo.sectionOffset + labelReference.byteOffset;
				addr.sectionIndex      = 0;
				for (; addr.sectionIndex <= lco.sections.size(); addr.sectionIndex++) {
					if (addr.sectionIndex == lco.sections.size()) {
						Logger::log(LogSeverity::Error, "Label reference defined at {}:{} is corrupted with invalid section index, please re-assemble", lineInfo.filename, lineInfo.fileLineNumber);
						return AssemblerError::INVALID_LABEL_REFERENCE;
					}

					if (lco.sections[addr.sectionIndex].name == lineInfo.sectionName)
						break;
				}
				for (; addr.symbolIndex < lco.symbols.size(); addr.symbolIndex++) {
					if (addr.symbolIndex == lco.symbols.size()) {
						Logger::log(LogSeverity::Error, "Label reference defined at {}:{} is corrupted with invalid symbol index, please re-assemble", lineInfo.filename, lineInfo.fileLineNumber);
						return AssemblerError::INVALID_LABEL_REFERENCE;
					}

					if (lco.symbols[addr.symbolIndex].name == labelReference.labelName)
						break;
				}
				lco.symbolAddrs.push_back(addr);
			}
		}

		return AssemblerError::GOOD;
	}
} // namespace Assembler