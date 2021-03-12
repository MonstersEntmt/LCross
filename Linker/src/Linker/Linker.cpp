#include "Linker/Linker.h"

#include <Utils/ArgUtils.h>
#include <Utils/PrintUtils.h>

namespace Linker {
	Format sLinkerFormat = Format::DEFAULT;

	static void handleFormatArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
		std::string argValue = values[0];
		if (argValue == "default") sLinkerFormat = Format::DEFAULT;
#if _TARGETS_PE_
		else if (argValue == "pe") sLinkerFormat = Format::PE;
#endif
#if _TARGETS_ELF_
		else if (argValue == "elf") sLinkerFormat = Format::ELF;
#endif
#if _TARGETS_BIN_
		else if (argValue == "bin") sLinkerFormat = Format::BIN;
#endif
	}

	void addInputArgs(ArgUtils& argUtils) {
		argUtils.addHandler("-f", &handleFormatArg);
		argUtils.addArgInfo("-f", "Select output format");
		argUtils.addArgValue("-f", "format", "R");
		argUtils.addArgValue("-f", "format", "default");
#if _TARGETS_PE_
		argUtils.addArgValue("-f", "format", "pe");
#endif
#if _TARGETS_ELF_
		argUtils.addArgValue("-f", "format", "elf");
#endif
#if _TARGETS_BIN_
		argUtils.addArgValue("-f", "format", "bin");
#endif
	}

	void handleArgs() {
		if (sLinkerFormat == Format::DEFAULT) sLinkerFormat = getDefaultCompileFormatForHost();
	}

	void handleOutputName(ArgUtils& argUtils, std::string& outputName) {
		outputName = outputName.substr(0, outputName.find_last_of('.'));
		switch (sLinkerFormat) {
		case Format::PE: outputName += ".exe"; break;
		case Format::ELF: outputName += ".o"; break;
		case Format::BIN: outputName += ".bin"; break;
		}
	}

	Format getFormat() { return sLinkerFormat; }
}