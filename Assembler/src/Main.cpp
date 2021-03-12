#include "Assembler/Assembler.h"
#ifndef _NO_LINKER_
#include <Linker/Linker.h>
#endif

#include <Utils/ArgUtils.h>
#include <Utils/PrintUtils.h>

void handleOutputName(ArgUtils& argUtils, std::string& outputName) {
#ifndef _NO_LINKER_
	if (argUtils.getNoLink()) {
		outputName += ".o";
	} else {
		Linker::handleOutputName(argUtils, outputName);
	}
#else
	outputName += ".o";
#endif
}

namespace PrintUtils {
	std::ostream& appName(std::ostream& ostream) { return ostream << "LASM"; }
	std::ostream& appVersionInfo(std::ostream& ostream) {
		return ostream;
	}
}

int main(int argc, char** argv) {
	PrintUtils::setupAnsi();
	ArgUtils argUtils(argc, argv);
#ifdef _NO_LINKER_
	argUtils.setLinks(false);
	argUtils.setHasNoLinkArg(false);
#else
	Linker::addInputArgs(argUtils);
#endif
	argUtils.handle();
	if (argUtils.hasFailed()) {
		PrintUtils::restoreAnsi();
		return EXIT_FAILURE;
	}
#ifndef _NO_LINKER_
	Linker::handleArgs();
#endif
	argUtils.handleArgs();

	if (argUtils.isVerbose()) {
		printHostPlatform();
		printHostArch();

#ifndef _NO_LINKER_
		std::cout << PrintUtils::appInfo << "Current output format is " << PrintUtils::colorSchemeArg << Linker::getFormat() << PrintUtils::normal << std::endl;
#endif
		std::cout << PrintUtils::appInfo << "Current output arch is " << PrintUtils::colorSchemeArg << argUtils.getOutputArch() << PrintUtils::normal << std::endl;
		std::cout << PrintUtils::appInfo << "Current output filename is " << PrintUtils::colorSchemeArg << "'" << argUtils.getOutputName() << "'" << PrintUtils::normal << std::endl;

		auto& inputNames = argUtils.getInputNames();
		if (inputNames.size() > 1) {
			std::cout << PrintUtils::appInfo << "Current input filenames are:" << PrintUtils::colorSchemeArg;
			for (size_t i = 0; i < inputNames.size(); i++)
				std::cout << std::endl << "    '" << inputNames[i] << "'";
			std::cout << PrintUtils::normal;
		} else {
			std::cout << PrintUtils::appInfo << "Current input filename is " << PrintUtils::colorSchemeArg << "'" << inputNames[0] << "'" << PrintUtils::normal << std::endl;
		}
	}

	PrintUtils::restoreAnsi();
	return EXIT_SUCCESS;
	}