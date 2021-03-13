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

void AssemblerArgUtils::handleVersionFlag(ArgUtils & argUtils, const std::string & arg, const std::vector<std::string>&argValues, size_t & usedValueCount, bool& argFailed) {
#ifdef _NO_LINKER_
	ArgUtils::handleVersionFlag(argUtils, arg, argValues, usedValueCount, argFailed);
#else
	LinkerArgUtils::handleVersionFlag(argUtils, arg, argValues, usedValueCount, argFailed);
#endif
	std::cout << PrintUtils::colorSchemeInfo << "This build supports these output architectures:";

	std::vector<std::string> supportedOutputArchitectures{
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
			std::cout << std::endl << PrintUtils::colorSchemeArg << "    '" << format << "'" << PrintUtils::colorSchemeInfo;
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
	std::cout << std::endl << "This build has a built in Linker" << PrintUtils::normal << std::endl;
#else
	std::cout << PrintUtils::normal << std::endl;
#endif
}

void AssemblerArgUtils::handleArchFlag(ArgUtils & argUtils, const std::string & arg, const std::vector<std::string>&argValues, size_t & usedValueCount, bool& argFailed) {
	std::string argValue = argValues[0];
	if (argValue == "default") this->outputArch = Arch::DEFAULT;
#if _TARGETS_X86_64_
	else if (argValue == "x86") this->outputArch = Arch::X86;
#endif
#if _TARGETS_X86_64_
	else if (argValue == "x86_64") this->outputArch = Arch::X86_64;
#endif
#if _TARGETS_ARM32_
	else if (argValue == "arm32") this->outputArch = Arch::ARM32;
#endif
#if _TARGETS_ARM64_
	else if (argValue == "arm64") this->outputArch = Arch::ARM64;
#endif
	usedValueCount = 1;
}

#ifndef _NO_LINKER_
void AssemblerArgUtils::handleNoLinkFlag(ArgUtils & argUtils, const std::string & arg, const std::vector<std::string>&argValues, size_t & usedValueCount, bool& argFailed) {
	this->noLink = true;
}
#endif

std::string AssemblerArgUtils::getFullUsageString(const FlagInfo * currentFlag) {
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

#ifndef _NO_LINKER_
void AssemblerArgUtils::handleOutputName() {
	if (this->noLink) {
		ArgUtils::handleOutputName();
	} else {
		LinkerArgUtils::handleOutputName();
	}
}
#endif

void AssemblerArgUtils::handleVirt() {
#ifndef _NO_LINKER_
	LinkerArgUtils::handleVirt();
#endif
	if (this->outputArch == Arch::DEFAULT)
		this->outputArch = getDefaultCompileArchForHost();
}

std::ostream& operator<<(std::ostream & ostream, AssemblerError error) {
	switch (error) {
	case AssemblerError::GOOD: return ostream << "Good";
	case AssemblerError::NOT_IMPLEMENTED: return ostream << "Assembler not implemented";
	case AssemblerError::INVALID_OUTPUT_ARCH: return ostream << "Invalid Output Architecture";
	case AssemblerError::INVALID_INSTRUCTION: return ostream << "Invalid Instruction";
	case AssemblerError::INVALID_LABEL: return ostream << "Good";
	default: return ostream << "Unknown";
	}
}

namespace Assembler {
	using AssemblerFunction = std::function<AssemblerError(const AssemblerOptions& options, const std::vector<std::string>& instruction, std::vector<uint8_t>& bytecode)>;

	void tokenize(const std::string& str, std::vector<std::vector<std::string_view>>& tokens) {

	}

	AssemblerError assemble(const AssemblerOptions& options, const std::string& fileContent, std::vector<uint8_t>& bytecode) {
		AssemblerFunction assemblerFunc = nullptr;
		switch (options.outputArch) {
#if _TARGETS_X86_
		case Arch::X86: assemblerFunc = &Architecture::X86::assemble; break;
#endif
#if _TARGETS_X86_64_
		case Arch::X86_64: assemblerFunc = &Architecture::X86_64::assemble; break;
#endif
#if _TARGETS_ARM32_
		case Arch::ARM32: assemblerFunc = &Architecture::Arm32::assemble; break;
#endif
#if _TARGETS_ARM64_
		case Arch::ARM64: assemblerFunc = &Architecture::Arm64::assemble; break;
#endif
		default:
			// TODO: Add error message
			return AssemblerError::INVALID_OUTPUT_ARCH;
		}

		std::string curFilename = "";
		size_t curLine = 0;

		std::vector<std::vector<std::string_view>> lines;
		tokenize(fileContent, lines);

		std::vector<LabelInfo> labelInfos;
		for (size_t i = 0; i < lines.size(); curLine++, i++) {
			std::vector<std::string_view>& tokens = lines[i];
			if (tokens.size() >= 2) {
				std::string_view& labelName = tokens[0];
				std::string_view& labelId = tokens[1];
				if (labelId == ":") {
					if (labelName != " ") {
						// TODO: Add error message
						return AssemblerError::INVALID_LABEL;
					}
					labelInfos.push_back({ std::string(labelName), curLine });
				}
			}
		}
		curLine = 0;

		return AssemblerError::GOOD;
	}
}