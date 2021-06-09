#include "CppCompiler/CppCompiler.h"

#include <Common/PrintUtils.h>

CppCompilerArgUtils::CppCompilerArgUtils(int argc, char** argv)
#ifdef _NO_ASSEMBLER_
	: ArgUtils(argc, argv) {
#else
	: AssemblerArgUtils(argc, argv) {
#endif
#ifdef _NO_ASSEMBLER_
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
	registerHandlerFunc("-a", std::bind(&CppCompilerArgUtils::handleArchFlag, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
#endif
}

void CppCompilerArgUtils::handleVersionFlag(ArgUtils & argUtils, const std::string & arg, const std::vector<std::string>&argValues, size_t & usedValueCount, bool& argFailed) {
#ifdef _NO_ASSEMBLER_
	ArgUtils::handleVersionFlag(argUtils, arg, argValues, usedValueCount, argFailed);
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
	std::cout << PrintUtils::normal << std::endl;
#else
	AssemblerArgUtils::handleVersionFlag(argUtils, arg, argValues, usedValueCount, argFailed);
	std::cout << PrintUtils::colorSchemeInfo << "This build has a built in Assembler" << PrintUtils::normal << std::endl;
#endif
}

#ifdef _NO_ASSEMBLER_
void CppCompilerArgUtils::handleArchFlag(ArgUtils & argUtils, const std::string & arg, const std::vector<std::string>&argValues, size_t & usedValueCount, bool& argFailed) {
	std::string argValue = argValues[0];
	if (argValue == "default") this->outputArch = OutputArch::DEFAULT;
#if _TARGETS_X86_64_
	else if (argValue == "x86") this->outputArch = OutputArch::X86;
#endif
#if _TARGETS_X86_64_
	else if (argValue == "x86_64") this->outputArch = OutputArch::X86_64;
#endif
#if _TARGETS_ARM32_
	else if (argValue == "arm32") this->outputArch = OutputArch::ARM32;
#endif
#if _TARGETS_ARM64_
	else if (argValue == "arm64") this->outputArch = OutputArch::ARM64;
#endif
	usedValueCount = 1;
}
#endif

void CppCompilerArgUtils::handleOutputName() {
#ifdef _NO_ASSEMBLER_
	this->outputName += ".oasm";
#else
	AssemblerArgUtils::handleOutputName();
#endif
}

#ifdef _NO_ASSEMBLER_
void CppCompilerArgUtils::handleVirt() {
	if (this->outputArch == OutputArch::DEFAULT)
		this->outputArch = getDefaultCompileArchForHost();
}
#endif

std::ostream& operator<<(std::ostream & ostream, CppCompilerError error) {
	switch (error) {
	case CppCompilerError::GOOD: return ostream << "Good";
	case CppCompilerError::NOT_IMPLEMENTED: return ostream << "C Compiler not implemented";
	default: return ostream << "Unknown";
	}
}

namespace CppCompiler {
	CppCompilerError compile(const CppCompilerOptions& options, const std::string& fileContent, std::string& assemblyCode) {
		return CppCompilerError::GOOD;
	}
}