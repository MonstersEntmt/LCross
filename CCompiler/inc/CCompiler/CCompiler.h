#pragma once

#include <Common/ArgUtils.h>
#include <Core.h>

#include <string>
#include <vector>

#ifndef _NO_ASSEMBLER_
#include <Assembler/Assembler.h>
#endif

#ifdef _NO_ASSEMBLER_
class CCompilerArgUtils : public ArgUtils {
#else
class CCompilerArgUtils : public AssemblerArgUtils {
#endif
public:
	CCompilerArgUtils(int argc, char** argv);

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

enum class CCompilerError : uint32_t {
	GOOD = 0,
	NOT_IMPLEMENTED
};

std::ostream& operator<<(std::ostream& ostream, CCompilerError error);

struct CCompilerOptions {
	bool verbose = false;
	OutputArch outputArch = OutputArch::DEFAULT;
};

namespace CCompiler {
	CCompilerError compile(const CCompilerOptions& options, const std::string& fileContent, std::string& assemblyCode);
}