#include "Linker/Linker.h"

#include <Common/PrintUtils.h>
#include <Core.h>

#if _TARGETS_PE_
	#include "Linker/Format/PEFormat.h"
#endif
#if _TARGETS_ELF_
	#include "Linker/Format/ELFFormat.h"
#endif
#if _TARGETS_BIN_
	#include "Linker/Format/BinFormat.h"
#endif

LinkerArgUtils::LinkerArgUtils(int argc, char** argv)
    : ArgUtils(argc, argv) {
	registerFlagInfo(FlagInfo("-f", "Select output format", "", { FlagValueInfo("format", FlagValueType::REQUIRED, {
		                          "default",
#if _TARGETS_PE_
		                              "pe",
#endif
#if _TARGETS_ELF_
		                              "elf",
#endif
#if _TARGETS_BIN_
		                              "bin",
#endif
	                          }) }));
	registerHandlerFunc("-f", std::bind(&LinkerArgUtils::handleFormatFlag, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void LinkerArgUtils::handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	ArgUtils::handleVersionFlag(argUtils, arg, argValues, usedValueCount, argFailed);
	std::cout << PrintUtils::colorSchemeInfo << "This build supports these output formats:";

	std::vector<std::string> supportedOutputFormats {
#if _TARGETS_PE_
		"pe",
#endif
#if _TARGETS_ELF_
		    "elf",
#endif
#if _TARGETS_BIN_
		    "bin",
#endif
	};

	size_t numChars = 0;
	for (size_t i = 0; i < supportedOutputFormats.size(); i++)
		numChars += supportedOutputFormats[i].length();

	if (numChars > 50) {
		for (std::string& format : supportedOutputFormats)
			std::cout << std::endl
			          << PrintUtils::colorSchemeArg << "    '" << format << "'" << PrintUtils::colorSchemeInfo;
	} else {
		for (size_t i = 0; i < supportedOutputFormats.size(); i++) {
			if (i == 0)
				std::cout << " ";
			else
				std::cout << ", ";
			std::cout << PrintUtils::colorSchemeArg << "'" << supportedOutputFormats[i] << "'" << PrintUtils::colorSchemeInfo;
		}
	}
	std::cout << PrintUtils::normal << std::endl;
}

void LinkerArgUtils::handleFormatFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	std::string argValue = argValues[0];
	if (argValue == "default") this->outputFormat = Format::DEFAULT;
#if _TARGETS_PE_
	else if (argValue == "pe")
		this->outputFormat = Format::PE;
#endif
#if _TARGETS_ELF_
	else if (argValue == "elf")
		this->outputFormat = Format::ELF;
#endif
#if _TARGETS_BIN_
	else if (argValue == "bin")
		this->outputFormat = Format::BIN;
#endif
	usedValueCount = 1;
}

void LinkerArgUtils::handleOutputName() {
	this->outputName = this->outputName.substr(0, this->outputName.find_last_of('.'));
	switch (this->outputFormat) {
	case Format::PE: this->outputName += ".exe"; break;
	case Format::ELF: this->outputName += ".o"; break;
	case Format::BIN: this->outputName += ".bin"; break;
	}
}

void LinkerArgUtils::handleVirt() {
	if (this->outputFormat == Format::DEFAULT)
		this->outputFormat = getDefaultCompileFormatForHost();
}

std::ostream& operator<<(std::ostream& ostream, LinkerError linkerError) {
	switch (linkerError) {
	case LinkerError::GOOD: return ostream << "Good";
	case LinkerError::NOT_IMPLEMENTED: return ostream << "Linker not implemented";
	case LinkerError::INVALID_OUTPUT_FORMAT: return ostream << "Invalid output format";
	case LinkerError::INVALID_OUTPUT_ARCH: return ostream << "Invalid output arch";
	default: return ostream << "Unknown";
	}
}

namespace Linker {
	using LinkerFunction = std::function<LinkerError(const LinkerOptions& options, ByteBuffer& bytecode)>;

	LinkerError link(const LinkerOptions& options, ByteBuffer& bytecode) {
		LinkerFunction linkerFunc = nullptr;
		switch (options.outputFormat) {
#if _TARGETS_PE_
		case Format::PE: linkerFunc = &Formats::PE::link; break;
#endif
#if _TARGETS_ELF_
		case Format::ELF: linkerFunc = &Formats::ELF::link; break;
#endif
#if _TARGETS_BIN_
		case Format::BIN: linkerFunc = &Formats::Bin::link; break;
#endif
		default:
			// TODO: Add error message
			return LinkerError::INVALID_OUTPUT_FORMAT;
		}
		return linkerFunc(options, bytecode);
	}
} // namespace Linker