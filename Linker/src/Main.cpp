#include "Linker/Linker.h"

#include <Utils/ArgUtils.h>
#include <Utils/PrintUtils.h>

void handleOutputName(ArgUtils& argUtils, std::string& outputName) { Linker::handleOutputName(argUtils, outputName); }

namespace PrintUtils {
	std::ostream& appName(std::ostream& ostream) { return ostream << "LLINK"; }
	std::ostream& appVersionInfo(std::ostream& ostream) {
		return ostream;
	}
}

int main(int argc, char** argv) {
	PrintUtils::setupAnsi();
	ArgUtils argUtils(argc, argv);
	Linker::addInputArgs(argUtils);
	argUtils.setHasNoLinkArg(false);
	
	argUtils.handle();
	if (argUtils.hasFailed()) {
		PrintUtils::restoreAnsi();
		return EXIT_FAILURE;
	}

	Linker::handleArgs();
	argUtils.handleArgs();

	if (argUtils.hasFailed()) {
		PrintUtils::restoreAnsi();
		return EXIT_FAILURE;
	}

	if (argUtils.isVerbose()) {
		printHostPlatform();
		printHostArch();

		std::cout << PrintUtils::appInfo << "Current output format is " << PrintUtils::colorSchemeArg << Linker::getFormat() << PrintUtils::normal << std::endl;
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