#include "Linker/Linker.h"

#include <Common/FileUtils.h>
#include <Common/Logger.h>
#include <Common/PrintUtils.h>

namespace PrintUtils {
	std::string appName() { return "LLINK"; }
	std::string appVersion() { return "0.1.0"; }
} // namespace PrintUtils

int main(int argc, char** argv) {
	try {
		PrintUtils::setupAnsi();
		LinkerArgUtils argUtils(argc, argv);
		argUtils.handle();

		if (argUtils.verbose) {
			printHostPlatform();
			printHostArch();

			Logger::log(LogSeverity::Info, "Current output format is {}'{}'{}", LogColors::Arg, argUtils.outputFormat, LogColors::Info);
			Logger::log(LogSeverity::Info, "Current output filename is {}'{}'{}", LogColors::Arg, argUtils.outputName, LogColors::Info);

			auto& inputNames = argUtils.inputNames;
			if (inputNames.size() > 1) {
				Logger::log(LogSeverity::Info, "Current input filenames are:");
				for (size_t i = 0; i < inputNames.size(); i++)
					Logger::print("{}    '{}'", LogColors::Arg, inputNames[i]);
			} else {
				Logger::log(LogSeverity::Info, "Current input filename is {}'{}'{}", LogColors::Arg, inputNames[0], LogColors::Info);
			}
		}

		auto& inputNames              = argUtils.inputNames;
		const std::string& outputName = argUtils.outputName;
		FileUtils::deleteFile(outputName);

		Linker::LinkerState state;
		state.options.verbose      = argUtils.verbose;
		state.options.outputFormat = argUtils.outputFormat;
		state.options.inputFiles.resize(inputNames.size());
		for (size_t i = 0; i < inputNames.size(); i++)
			FileUtils::readLCO(inputNames[i], state.options.inputFiles[i]);

		ByteBuffer bytecode;
		LinkerError error = Linker::link(state, bytecode);
		if (error != LinkerError::GOOD) {
			Logger::log(LogSeverity::Error, "Linker failed with error {}'{}'{}", LogColors::Arg, error, LogColors::Error);
			PrintUtils::restoreAnsi();
			return EXIT_FAILURE;
		}
		FileUtils::writeFileBinary(outputName, bytecode);

		if (argUtils.verbose)
			Logger::log(LogSeverity::Info, "Linker succeeded");
	} catch (std::exception e) {
		PrintUtils::restoreAnsi();
		return EXIT_FAILURE;
	}
	PrintUtils::restoreAnsi();
	return EXIT_SUCCESS;
}