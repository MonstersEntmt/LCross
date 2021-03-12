#include <Core.h>
#include <Utils/ArgUtils.h>
#include <Utils/PrintUtils.h>
#include <iostream>

static Format assemblerFormat = Format::DEFAULT;
static Arch assemblerArch = Arch::DEFAULT;
static bool assemblerLinkInputs = true;
static std::vector<std::string> assemblerInputs;
static std::string assemblerOutput;

namespace PrintUtils {
	std::ostream& appName(std::ostream& ostream) { return ostream << "LASM"; }
}

std::string getUsageString(ArgUtils& argUtils) {
	auto& arguments = argUtils.getArguments();
	auto helpArgument = std::find(arguments.begin(), arguments.end(), "-h");
	if (helpArgument != arguments.end()) {
		helpArgument++;
		if (helpArgument != arguments.end() && (*helpArgument) == "no_link")
			return "<input filename>";
		else
			return "<input filenames ...>";
	} else {
		if (assemblerLinkInputs)
			return "<input filenames ...>";
		else
			return "-no_link <input filename>";
	}
}

static void handleDefaultArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
	assemblerInputs.push_back(arg);
}

static void handleFormatArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
	std::string argValue = values[0];
	if (argValue == "default") assemblerFormat = Format::DEFAULT;
#if _TARGETS_PE_
	else if (argValue == "pe") assemblerFormat = Format::PE;
#endif
#if _TARGETS_ELF_
	else if (argValue == "elf") assemblerFormat = Format::ELF;
#endif
#if _TARGETS_BIN_
	else if (argValue == "bin") assemblerFormat = Format::BIN;
#endif
	usedValueCount = 1;
}

static void handleArchArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
	std::string argValue = values[0];
	if (argValue == "default") assemblerArch = Arch::DEFAULT;
#if _TARGETS_X86_
	else if (argValue == "pe") assemblerArch = Arch::X86;
#endif
#if _TARGETS_X86_64_
	else if (argValue == "elf") assemblerArch = Arch::X86_64;
#endif
#if _TARGETS_ARM32_
	else if (argValue == "bin") assemblerArch = Arch::ARM32;
#endif
#if _TARGETS_ARM64_
	else if (argValue == "bin") assemblerArch = Arch::ARM64;
#endif
	usedValueCount = 1;
}

static void handleOutputArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
	assemblerOutput = values[0];
	usedValueCount = 1;
}

static void handleNoLinkArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
	assemblerLinkInputs = false;
}

int main(int argc, char** argv) {
	PrintUtils::setupAnsi();
	ArgUtils argUtils(argc, argv);
	argUtils.addHandler("", &handleDefaultArg);
	argUtils.addHandler("-f", &handleFormatArg);
	argUtils.addHandler("-a", &handleArchArg);
	argUtils.addHandler("-o", &handleOutputArg);
	argUtils.addHandler("-no_link", &handleNoLinkArg);
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
	argUtils.addArgInfo("-a", "Select output architecture");
	argUtils.addArgValue("-a", "architecture", "R");
	argUtils.addArgValue("-a", "architecture", "default");
#if _TARGETS_X86_
	argUtils.addArgValue("-a", "architecture", "x86");
#endif
#if _TARGETS_X86_64_
	argUtils.addArgValue("-a", "architecture", "x86_64");
#endif
#if _TARGETS_ARM32_
	argUtils.addArgValue("-a", "architecture", "arm32");
#endif
#if _TARGETS_ARM64_
	argUtils.addArgValue("-a", "architecture", "arm64");
#endif
	argUtils.addArgInfo("-o", "Set output filename");
	argUtils.addArgValue("-o", "output filename", "R");
	argUtils.addArgInfo("-no_link", "Do not link the file inputs", "This makes '<input filenames ...>' syntax wrong as it uses '<input filename>' syntax instead");
	argUtils.handle();
	if (argUtils.hasFailed()) {
		PrintUtils::restoreAnsi();
		return EXIT_FAILURE;
	}

	if (assemblerFormat == Format::DEFAULT) assemblerFormat = getDefaultCompileFormatForHost();
	if (assemblerArch == Arch::DEFAULT) assemblerArch = getDefaultCompileArchForHost();

	if (assemblerOutput.empty()) {
		if (assemblerLinkInputs && assemblerInputs.size() > 1) {
			std::cout << PrintUtils::appError << "Missing output filename!" << PrintUtils::normal << std::endl;
			std::cout << PrintUtils::appUsage << "'\"" << argUtils.getCommand() << "\" " << getFullUsageString(argUtils, nullptr) << "'" << PrintUtils::normal << std::endl;
			PrintUtils::restoreAnsi();
			return EXIT_FAILURE;
		}
		assemblerOutput = assemblerInputs[0];
		if (assemblerLinkInputs) {
			assemblerOutput = assemblerOutput.substr(0, assemblerOutput.find_last_of('.'));
			switch (assemblerFormat) {
			case Format::PE: assemblerOutput += ".exe"; break;
			case Format::ELF: assemblerOutput += ".o"; break;
			case Format::BIN: assemblerOutput += ".bin"; break;
			}
		} else {
			switch (assemblerFormat) {
			case Format::PE: assemblerOutput += ".obj"; break;
			case Format::ELF: assemblerOutput += ".o"; break;
			case Format::BIN: assemblerOutput += ".bin"; break;
			}
		}
	}

	printHostPlatform();
	printHostArch();

	std::cout << PrintUtils::appInfo << "Current output format is " << PrintUtils::colorSchemeArg;
	switch (assemblerFormat) {
	case Format::PE: std::cout << "pe"; break;
	case Format::ELF: std::cout << "elf"; break;
	case Format::BIN: std::cout << "bin"; break;
	}
	std::cout << PrintUtils::normal << std::endl;

	std::cout << PrintUtils::appInfo << "Current output arch is " << PrintUtils::colorSchemeArg;
	switch (assemblerArch) {
	case Arch::X86: std::cout << "x86"; break;
	case Arch::X86_64: std::cout << "x86_64"; break;
	case Arch::ARM32: std::cout << "arm32"; break;
	case Arch::ARM64: std::cout << "arm64"; break;
	}
	std::cout << PrintUtils::normal << std::endl;

	std::cout << PrintUtils::appInfo << "Current output filename is " << PrintUtils::colorSchemeArg << "'" << assemblerOutput << "'" << PrintUtils::normal << std::endl;

	if (assemblerLinkInputs && assemblerInputs.size() > 1) {
		std::cout << PrintUtils::appInfo << "Current input filenames are:" << PrintUtils::colorSchemeArg;
		for (size_t i = 0; i < assemblerInputs.size(); i++)
			std::cout << std::endl << "    '" << assemblerInputs[i] << "'";
		std::cout << PrintUtils::normal;
	} else {
		std::cout << PrintUtils::appInfo << "Current input filename is " << PrintUtils::colorSchemeArg << "'" << assemblerInputs[0] << "'" << PrintUtils::normal << std::endl;
	}

	PrintUtils::restoreAnsi();
	return EXIT_SUCCESS;
}