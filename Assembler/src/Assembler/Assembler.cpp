#include "Assembler/Assembler.h"

#include <Common/PrintUtils.h>
#include <Core.h>

#if _TARGETS_X86_
	#include "Assembler/Arch/X86Assembler.h"
#endif
#if _TARGETS_X86_64_
	#include "Assembler/Arch/X86_64Assembler.h"
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
	std::cout << PrintUtils::colorSchemeInfo << "This build supports these output architectures:";

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

	size_t numChars = 0;
	for (size_t i = 0; i < supportedOutputArchitectures.size(); i++)
		numChars += supportedOutputArchitectures[i].length();

	if (numChars > 50) {
		for (std::string& format : supportedOutputArchitectures)
			std::cout << std::endl
			          << PrintUtils::colorSchemeArg << "    '" << format << "'" << PrintUtils::colorSchemeInfo;
	} else {
		for (size_t i = 0; i < supportedOutputArchitectures.size(); i++) {
			if (i == 0)
				std::cout << " ";
			else
				std::cout << ", ";
			std::cout << PrintUtils::colorSchemeArg << "'" << supportedOutputArchitectures[i] << "'" << PrintUtils::colorSchemeInfo;
		}
	}
#ifndef _NO_LINKER_
	std::cout << std::endl
	          << "This build has a built in Linker" << PrintUtils::normal << std::endl;
#else
	std::cout << PrintUtils::normal << std::endl;
#endif
}

void AssemblerArgUtils::handleArchFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	std::string argValue = argValues[0];
	if (argValue == "default") this->outputArch = Arch::DEFAULT;
#if _TARGETS_X86_64_
	else if (argValue == "x86")
		this->outputArch = Arch::X86;
#endif
#if _TARGETS_X86_64_
	else if (argValue == "x86_64")
		this->outputArch = Arch::X86_64;
#endif
#if _TARGETS_ARM32_
	else if (argValue == "arm32")
		this->outputArch = Arch::ARM32;
#endif
#if _TARGETS_ARM64_
	else if (argValue == "arm64")
		this->outputArch = Arch::ARM64;
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
	if (this->outputArch == Arch::DEFAULT)
		this->outputArch = getDefaultCompileArchForHost();
}

std::ostream& operator<<(std::ostream& ostream, AssemblerError error) {
	switch (error) {
	case AssemblerError::GOOD: return ostream << "Good";
	case AssemblerError::NOT_IMPLEMENTED: return ostream << "Assembler not implemented";
	case AssemblerError::INVALID_OUTPUT_ARCH: return ostream << "Invalid Output Architecture";
	case AssemblerError::INVALID_INSTRUCTION: return ostream << "Invalid Instruction";
	case AssemblerError::INVALID_LABEL: return ostream << "Invalid Label";
	case AssemblerError::SETUP_CORRUPTED: return ostream << "Setup Corrupted";
	default: return ostream << "Unknown";
	}
}

namespace Assembler {
	using AssemblerFunction = std::function<AssemblerError(const AssemblerOptions& options, const std::vector<std::string>& instruction, ByteBuffer& bytes)>;

	AssemblerError tokenize(const std::string& str, std::vector<std::vector<std::string>>& tokens) {
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
			if (line.length() == 0)
				continue;

			size_t commentBegin   = line.find_first_of(';');
			if (commentBegin < line.length())
				line = line.substr(0, commentBegin);

			size_t offset   = line.find_first_not_of(' ');
			size_t labelEnd = line.find_first_of(':');
			if (labelEnd < line.length()) {
				tokenLine.push_back(std::string(line.substr(offset, labelEnd)));
				tokenLine.push_back(":");
				offset = line.find_first_not_of(' ', labelEnd + 1);
			}

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

		return AssemblerError::GOOD;
	}

	AssemblerError assemble(const AssemblerOptions& options, const std::string& fileContent, LCO& lco) {
		AssemblerFunction assemblerFunc = nullptr;
		switch (options.outputArch) {
#if _TARGETS_X86_
		case Arch::X86: {
			AssemblerError error = Archs::X86::setupAssembler();
			if (error != AssemblerError::GOOD)
				return error;
			assemblerFunc = &Archs::X86::assemble;
			break;
		}
#endif
#if _TARGETS_X86_64_
		case Arch::X86_64:
			assemblerFunc = &Archs::X86_64::assemble;
			break;
#endif
#if _TARGETS_ARM32_
		case Arch::ARM32:
			assemblerFunc = &Archs::Arm32::assemble;
			break;
#endif
#if _TARGETS_ARM64_
		case Arch::ARM64:
			assemblerFunc = &Archs::Arm64::assemble;
			break;
#endif
		default:
			// TODO: Add error message
			return AssemblerError::INVALID_OUTPUT_ARCH;
		}

		std::string curFilename = "";
		size_t curLine          = 0;

		std::vector<std::vector<std::string>> lines;
		AssemblerError error = tokenize(fileContent, lines);
		if (error != AssemblerError::GOOD)
			return error;

		std::vector<LabelInfo> labelInfos;
		std::string pLabelName;
		for (curLine = 0; curLine < lines.size(); curLine++) {
			std::vector<std::string>& tokens = lines[curLine];
			if (tokens.size() >= 2) {
				std::string& labelName = tokens[0];
				std::string& labelId   = tokens[1];
				if (labelId == ":") {
					std::string ln = labelName;
					if (ln[0] == '.')
						ln = pLabelName + ln;
					labelInfos.push_back({ ln, curLine });
					pLabelName = ln;
				}
			}
		}

		std::vector<LineInfo> lineInfos;
		std::string currentSectionName = ".text";
		lineInfos.resize(lines.size());
		for (curLine = 0; curLine < lines.size(); curLine++) {
			std::vector<std::string>& tokens = lines[curLine];
			size_t currentToken              = 0;
			if (tokens.size() >= 2) {
				std::string& labelName = tokens[0];
				std::string& labelId   = tokens[1];
				if (labelId == ":") {
					pLabelName = labelName;
					currentToken += 2;
				}
			}

			if (currentToken < tokens.size()) {
				std::string& instructionName = tokens[currentToken++];

				std::vector<std::string> instruction;
				instruction.push_back(instructionName);
				for (; currentToken < tokens.size(); currentToken++) {
					instruction.push_back(tokens[currentToken]);
				}
				LineInfo& lineInfo   = lineInfos[curLine];
				lineInfo.sectionName = currentSectionName;
				AssemblerError error = assemblerFunc(options, instruction, lineInfo.bytes);
				if (error != AssemblerError::GOOD)
					return error;
			}
		}

		lco.outputArch = options.outputArch;
		for (LineInfo& lineInfo : lineInfos) {
			Section* section = lco.getSection(lineInfo.sectionName);
			if (!section) {
				lco.sections.push_back(Section(lineInfo.sectionName));
				section = lco.getSection(lineInfo.sectionName);
			}
			section->bytes.addBytes(lineInfo.bytes);
		}

		for (LabelInfo& labelInfo : labelInfos) {
			Symbol labelSymbol(labelInfo.name, SymbolType::LABEL);
			if (labelInfo.lineNumber >= lineInfos.size()) {
				// TODO: Add error message
				return AssemblerError::INVALID_LABEL;
			}

			const std::string& sectionName = lineInfos[labelInfo.lineNumber].sectionName;
			labelSymbol.sectionIndex       = 0;
			for (; labelSymbol.sectionIndex <= lco.sections.size(); labelSymbol.sectionIndex++) {
				if (labelSymbol.sectionIndex == lco.sections.size()) {
					// TODO: Add error message
					return AssemblerError::INVALID_LABEL;
				}

				if (lco.sections[labelSymbol.sectionIndex].name == sectionName)
					break;
			}

			labelSymbol.sectionByteOffset = 0; // Calculate from the assembled binary.
			for (size_t i = 0; i < labelInfo.lineNumber; i++)
				if (lineInfos[i].sectionName == sectionName)
					labelSymbol.sectionByteOffset += lineInfos[i].bytes.size();

			labelSymbol.sectionByteLength = 0;
			lco.symbols.push_back(labelSymbol);
		}

		return AssemblerError::GOOD;
	}
} // namespace Assembler