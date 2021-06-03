#include "CppCompiler/CppCompiler.h"

#include <Common/PrintUtils.h>
#include <Common/FileUtils.h>

namespace PrintUtils {
	std::ostream& appName(std::ostream& ostream) { return ostream << "LCPP"; }
	std::ostream& appVersionInfo(std::ostream& ostream) {
		return ostream;
	}
}

int main(int argc, char** argv) {
	try {
		PrintUtils::setupAnsi();
		CppCompilerArgUtils argUtils(argc, argv);
		argUtils.handle();

		if (argUtils.verbose) {
			printHostPlatform();
			printHostArch();

#ifndef _NO_LINKER_
			std::cout << PrintUtils::appInfo << "Current output format is " << PrintUtils::colorSchemeArg << argUtils.outputFormat << PrintUtils::normal << std::endl;
#endif
			std::cout << PrintUtils::appInfo << "Current output arch is " << PrintUtils::colorSchemeArg << argUtils.outputArch << PrintUtils::normal << std::endl;
			std::cout << PrintUtils::appInfo << "Current output filename is " << PrintUtils::colorSchemeArg << "'" << argUtils.outputName << "'" << PrintUtils::normal << std::endl;

			auto& inputNames = argUtils.inputNames;
			if (inputNames.size() > 1) {
				std::cout << PrintUtils::appInfo << "Current input filenames are:" << PrintUtils::colorSchemeArg;
				for (size_t i = 0; i < inputNames.size(); i++)
					std::cout << std::endl << "    '" << inputNames[i] << "'";
				std::cout << PrintUtils::normal;
			}
			else {
				std::cout << PrintUtils::appInfo << "Current input filename is " << PrintUtils::colorSchemeArg << "'" << inputNames[0] << "'" << PrintUtils::normal << std::endl;
			}
		}

		auto& inputNames = argUtils.inputNames;
		const std::string& outputName = argUtils.outputName;
		FileUtils::deleteFile(outputName);

		CppCompilerOptions compilerOptions;
		compilerOptions.outputArch = argUtils.outputArch;
		compilerOptions.verbose = argUtils.verbose;

#ifdef _NO_ASSEMBLER_
		std::string fileContent;
		FileUtils::readFile(inputNames[0], fileContent);
		std::string assemblyContent;
		CppCompilerError error = CppCompiler::compile(compilerOptions, fileContent, assemblyContent);
		if (error != CppCompilerError::GOOD) {
			std::cout << PrintUtils::appError << "Cpp Compiler failed with error " << PrintUtils::colorSchemeArg << "'" << error << "'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
			PrintUtils::restoreAnsi();
			return EXIT_FAILURE;
		}
		FileUtils::writeFile(outputName, assemblyContent);

		if (argUtils.verbose)
			std::cout << PrintUtils::appInfo << "Cpp Compiler succeeded" << PrintUtils::normal << std::endl;
#else
		AssemblerOptions assemblerOptions;
		assemblerOptions.outputArch = argUtils.outputArch;
		assemblerOptions.verbose = argUtils.verbose;
#ifdef _NO_LINKER_
		std::string fileContent;
		FileUtils::readFile(inputNames[0], fileContent);
		std::string assemblyContent;
		CppCompilerError error = CppCompiler::compile(compilerOptions, fileContent, assemblyContent);
		if (error != CppCompilerError::GOOD) {
			std::cout << PrintUtils::appError << "Cpp Compiler failed with error " << PrintUtils::colorSchemeArg << "'" << error << "'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
			PrintUtils::restoreAnsi();
			return EXIT_FAILURE;
		}

		if (argUtils.verbose)
			std::cout << PrintUtils::appInfo << "Cpp Compiler succeeded" << PrintUtils::normal << std::endl;

		std::vector<uint8_t> bytecode;
		AssemblerError asError = Assembler::assemble(assemblerOptions, assemblyContent, bytecode);
		if (asError != AssemblerError::GOOD) {
			std::cout << PrintUtils::appError << "Assembler failed with error " << PrintUtils::colorSchemeArg << "'" << asError << "'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
			PrintUtils::restoreAnsi();
			return EXIT_FAILURE;
		}
		FileUtils::writeFileBinary(outputName, bytecode);

		if (argUtils.verbose)
			std::cout << PrintUtils::appInfo << "Assembler succeeded" << PrintUtils::normal << std::endl;
#else
		std::vector<std::string> assemblyFiles;
		assemblyFiles.resize(inputNames.size());
		for (size_t i = 0; i < inputNames.size(); i++) {
			std::string fileContent;
			FileUtils::readFile(inputNames[i], fileContent);
			CppCompilerError error = CppCompiler::compile(compilerOptions, fileContent, assemblyFiles[i]);
			if (error != CppCompilerError::GOOD) {
				std::cout << PrintUtils::appError << "Cpp Compiler failed with error " << PrintUtils::colorSchemeArg << "'" << error << "'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
				PrintUtils::restoreAnsi();
				return EXIT_FAILURE;
			}
		}

		if (argUtils.verbose)
			std::cout << PrintUtils::appInfo << "Cpp Compiler succeeded" << PrintUtils::normal << std::endl;

		std::vector<LCO> lcos;
		for (auto& assemblyFile : assemblyFiles) {
			LCO lco;
			AssemblerError error = Assembler::assemble(assemblerOptions, assemblyFile, lco);
			if (error != AssemblerError::GOOD) {
				std::cout << PrintUtils::appError << "Assembler failed with error " << PrintUtils::colorSchemeArg << "'" << error << "'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
				PrintUtils::restoreAnsi();
				return EXIT_FAILURE;
			}
			lcos.push_back(lco);
		}

		if (argUtils.verbose)
			std::cout << PrintUtils::appInfo << "Assembler succeeded" << PrintUtils::normal << std::endl;

		LinkerOptions linkerOptions;
		linkerOptions.outputFormat = argUtils.outputFormat;
		linkerOptions.verbose = argUtils.verbose;
		linkerOptions.inputFiles = lcos;
		ByteBuffer bytecode;
		LinkerError error = Linker::link(linkerOptions, bytecode);
		if (error != LinkerError::GOOD) {
			std::cout << PrintUtils::appError << "Linker failed with error " << PrintUtils::colorSchemeArg << "'" << error << "'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
			PrintUtils::restoreAnsi();
			return EXIT_FAILURE;
		}
		FileUtils::writeFileBinary(outputName, bytecode);

		if (argUtils.verbose)
			std::cout << PrintUtils::appInfo << "Linker succeeded" << PrintUtils::normal << std::endl;
#endif
#endif
	}
	catch (std::exception e) {
		PrintUtils::restoreAnsi();
		return EXIT_FAILURE;
	}

	PrintUtils::restoreAnsi();
	return EXIT_SUCCESS;
}