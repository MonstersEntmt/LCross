#pragma once

#include <Common/ArgUtils.h>
#include <Core.h>

#include <string>
#include <vector>

#ifndef _NO_ASSEMBLER_
#include <Assembler/Assembler.h>
#endif

#ifdef _NO_ASSEMBLER_
class CppCompilerArgUtils : public ArgUtils {
#else
class CppCompilerArgUtils : public AssemblerArgUtils {
#endif
public:
	CppCompilerArgUtils(int argc, char** argv);

	virtual void handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) override;
#ifdef _NO_ASSEMBLER_
	virtual void handleArchFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed);
#endif

	virtual void handleOutputName() override;

#ifdef _NO_ASSEMBLER_
	virtual void handleVirt() override;
#endif

public: // Passed Arg Data
#ifdef _NO_ASSEMBLER_
	OutputArch outputArch = OutputArch::DEFAULT;
#endif
};

enum class CppCompilerError : uint32_t {
	GOOD = 0,
	NOT_IMPLEMENTED
};

std::ostream& operator<<(std::ostream& ostream, CppCompilerError error);

struct CppCompilerOptions {
	bool verbose = false;
	OutputArch outputArch = OutputArch::DEFAULT;
};

namespace CppCompiler {
	CppCompilerError compile(const CppCompilerOptions& options, const std::string& fileContent, std::string& assemblyCode);
}