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

	std::string supportedFormats;
	size_t numChars = 0;
	for (size_t i = 0; i < supportedOutputFormats.size(); i++)
		numChars += supportedOutputFormats[i].length();

	if (numChars > 50) {
		std::string str;
		for (std::string& format : supportedOutputFormats)
			str += "\n    '" + format + "'";
		supportedFormats = Format::format("{}{}{}", LogColors::Arg, str, LogColors::Info);
	} else {
		for (size_t i = 0; i < supportedOutputFormats.size(); i++) {
			if (i == 0)
				supportedFormats += " ";
			else
				supportedFormats += ", ";
			supportedFormats += Format::format("{}'{}'{}", LogColors::Arg, supportedOutputFormats[i], LogColors::Info);
		}
	}

	Logger::print("{}This build supports these output formats:{}", LogColors::Info, supportedFormats);
}

void LinkerArgUtils::handleFormatFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	std::string argValue = argValues[0];
	if (argValue == "default") this->outputFormat = OutputFormat::DEFAULT;
#if _TARGETS_PE_
	else if (argValue == "pe")
		this->outputFormat = OutputFormat::PE;
#endif
#if _TARGETS_ELF_
	else if (argValue == "elf")
		this->outputFormat = OutputFormat::ELF;
#endif
#if _TARGETS_BIN_
	else if (argValue == "bin")
		this->outputFormat = OutputFormat::BIN;
#endif
	usedValueCount = 1;
}

void LinkerArgUtils::handleOutputName() {
	this->outputName = this->outputName.substr(0, this->outputName.find_last_of('.'));
	switch (this->outputFormat) {
	case OutputFormat::PE: this->outputName += ".exe"; break;
	case OutputFormat::ELF: this->outputName += ".o"; break;
	case OutputFormat::BIN: this->outputName += ".bin"; break;
	}
}

void LinkerArgUtils::handleVirt() {
	if (this->outputFormat == OutputFormat::DEFAULT)
		this->outputFormat = getDefaultCompileFormatForHost();
}

namespace Linker {
	using LinkerFunction = std::function<LinkerError(LinkerState& state, ByteBuffer& bytecode)>;

	LinkerError link(LinkerState& state, ByteBuffer& bytecode) {
		LinkerFunction linkerFunc = nullptr;
		switch (state.options.outputFormat) {
#if _TARGETS_PE_
		case OutputFormat::PE: linkerFunc = &Linker::PE::link; break;
#endif
#if _TARGETS_ELF_
		case OutputFormat::ELF: linkerFunc = &Linker::ELF::link; break;
#endif
#if _TARGETS_BIN_
		case OutputFormat::BIN: linkerFunc = &Linker::Bin::link; break;
#endif
		default:
			Logger::log(LogSeverity::Error, "Format {}'{}'{} not supported", LogColors::Arg, state.options.outputFormat, LogColors::Error);
			return LinkerError::INVALID_OUTPUT_FORMAT;
		}
		return linkerFunc(state, bytecode);
	}
} // namespace Linker