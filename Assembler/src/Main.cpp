#include "Assembler/Assembler.h"

#include <Common/FileUtils.h>
#include <Common/Logger.h>
#include <Common/PrintUtils.h>

namespace PrintUtils {
	std::string appName() { return "LASM"; }
	std::string appVersion() { return "0.1.0"; }
} // namespace PrintUtils

int main(int argc, char** argv) {
#ifdef EXCEPTION_HANDLING
	try {
#endif
		PrintUtils::setupAnsi();
		AssemblerArgUtils argUtils(argc, argv);
		argUtils.handle();

		if (argUtils.verbose) {
			printHostPlatform();
			printHostArch();

#ifndef _NO_LINKER_
			if (!argUtils.noLink)
				Logger::log(LogSeverity::Info, "Current output format is {}'{}'{}", LogColors::Arg, argUtils.outputFormat, LogColors::Info);
#endif
			Logger::log(LogSeverity::Info, "Current output arch is {}'{}'{}", LogColors::Arg, argUtils.outputArch, LogColors::Info);
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

#ifndef _NO_LINKER_
		if (argUtils.noLink) {
#endif
			Assembler::AssemblerState assemblerState;
			assemblerState.options.outputArch = argUtils.outputArch;
			assemblerState.options.verbose    = argUtils.verbose;

			std::string fileContent;
			FileUtils::readFile(inputNames[0], fileContent);
			fileContent = "#line 0, " + inputNames[0] + "\n" + fileContent;
			LCO lco;
			AssemblerError error = Assembler::assemble(assemblerState, fileContent, lco);
			if (error != AssemblerError::GOOD) {
				Logger::log(LogSeverity::Error, "Assembler failed with error {}'{}'{}", LogColors::Arg, error, LogColors::Error);
				PrintUtils::restoreAnsi();
				return EXIT_FAILURE;
			}
			FileUtils::writeLCO(outputName, lco);

			if (argUtils.verbose)
				Logger::log(LogSeverity::Info, "Assembler succeeded");
#ifndef _NO_LINKER_
		} else {
			std::vector<LCO> lcos;
			lcos.resize(inputNames.size());
			for (size_t i = 0; i < inputNames.size(); i++) {
				std::string fileContent;
				FileUtils::readFile(inputNames[i], fileContent);
				fileContent = "#line 0, " + inputNames[i] + "\n" + fileContent;

				Assembler::AssemblerState assemblerState;
				assemblerState.options.outputArch = argUtils.outputArch;
				assemblerState.options.verbose    = argUtils.verbose;

				AssemblerError error = Assembler::assemble(assemblerState, fileContent, lcos[i]);
				if (error != AssemblerError::GOOD) {
					Logger::log(LogSeverity::Error, "Assembler failed with error {}'{}'{}", LogColors::Arg, error, LogColors::Error);
					PrintUtils::restoreAnsi();
					return EXIT_FAILURE;
				}
			}

			if (argUtils.verbose)
				Logger::log(LogSeverity::Info, "Assembler succeeded");

			Linker::LinkerState linkerState;
			linkerState.options.outputFormat = argUtils.outputFormat;
			linkerState.options.verbose      = argUtils.verbose;
			linkerState.options.inputFiles   = lcos;
			ByteBuffer bytecode;
			LinkerError error = Linker::link(linkerState, bytecode);
			if (error != LinkerError::GOOD) {
				Logger::log(LogSeverity::Error, "Linker failed with error {}'{}'{}", LogColors::Arg, error, LogColors::Error);
				return EXIT_FAILURE;
			}
			FileUtils::writeFileBinary(outputName, bytecode);

			if (argUtils.verbose)
				Logger::log(LogSeverity::Info, "Linker succeeded");
		}
#endif
#ifdef EXCEPTION_HANDLING
	} catch (std::exception e) {
		Logger::log(LogSeverity::Error, "Exception was thrown: {}", e.what());
		PrintUtils::restoreAnsi();
		return EXIT_FAILURE;
	}
#endif
	PrintUtils::restoreAnsi();
	return EXIT_SUCCESS;
}