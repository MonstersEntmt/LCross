#include "Linker/Linker.h"

#include <Common/PrintUtils.h>
#include <Common/FileUtils.h>

namespace PrintUtils {
	std::ostream& appName(std::ostream& ostream) { return ostream << "LLINK"; }
	std::ostream& appVersionInfo(std::ostream& ostream) {
		return ostream;
	}
}

int main(int argc, char** argv) {
	try {
		PrintUtils::setupAnsi();
		LinkerArgUtils argUtils(argc, argv);
		argUtils.handle();

		if (argUtils.verbose) {
			printHostPlatform();
			printHostArch();

			std::cout << PrintUtils::appInfo << "Current output format is " << PrintUtils::colorSchemeArg << argUtils.outputFormat << PrintUtils::normal << std::endl;
			std::cout << PrintUtils::appInfo << "Current output filename is " << PrintUtils::colorSchemeArg << "'" << argUtils.outputName << "'" << PrintUtils::normal << std::endl;

			auto& inputNames = argUtils.inputNames;
			if (inputNames.size() > 1) {
				std::cout << PrintUtils::appInfo << "Current input filenames are:" << PrintUtils::colorSchemeArg;
				for (size_t i = 0; i < inputNames.size(); i++)
					std::cout << std::endl << "    '" << inputNames[i] << "'";
				std::cout << PrintUtils::normal << std::endl;
			} else {
				std::cout << PrintUtils::appInfo << "Current input filename is " << PrintUtils::colorSchemeArg << "'" << inputNames[0] << "'" << PrintUtils::normal << std::endl;
			}
		}

		auto& inputNames = argUtils.inputNames;
		const std::string& outputName = argUtils.outputName;
		FileUtils::deleteFile(outputName);

		LinkerOptions options;
		options.verbose = argUtils.verbose;
		options.outputFormat = argUtils.outputFormat;
		options.inputFiles.resize(inputNames.size());
		for (size_t i = 0; i < inputNames.size(); i++)
			FileUtils::readLCO(inputNames[i], options.inputFiles[i]);

		ByteBuffer bytecode;
		LinkerError error = Linker::link(options, bytecode);
		if (error != LinkerError::GOOD) {
			std::cout << PrintUtils::appError << "Linker failed with error " << PrintUtils::colorSchemeArg << "'" << error << "'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
			PrintUtils::restoreAnsi();
			return EXIT_FAILURE;
		}
		FileUtils::writeFileBinary(outputName, bytecode);

		if (argUtils.verbose)
			std::cout << PrintUtils::appInfo << "Linker succeeded" << PrintUtils::normal << std::endl;
	} catch (std::exception e) {
		PrintUtils::restoreAnsi();
		return EXIT_FAILURE;
	}

	PrintUtils::restoreAnsi();
	return EXIT_SUCCESS;
}