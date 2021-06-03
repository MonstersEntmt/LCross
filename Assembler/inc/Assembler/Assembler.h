#pragma once

#include <Common/ArgUtils.h>
#include <Common/LCO.h>
#include <Core.h>

#include <string>
#include <vector>

#ifndef _NO_LINKER_
	#include <Linker/Linker.h>
#endif

#ifdef _NO_LINKER_
class AssemblerArgUtils : public ArgUtils {
#else
class AssemblerArgUtils : public LinkerArgUtils {
#endif
public:
	AssemblerArgUtils(int argc, char** argv);

	virtual void handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) override;
	virtual void handleArchFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed);
#ifndef _NO_LINKER_
	virtual void handleNoLinkFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed);
#endif

	virtual std::string getFullUsageString(const FlagInfo* currentFlag) override;
	virtual void handleOutputName() override;

	virtual void handleVirt() override;

public: // Passed Arg Data
	Arch outputArch = Arch::DEFAULT;
#ifndef _NO_LINKER_
	bool noLink = false;
#endif
};

enum class AssemblerError : uint32_t {
	GOOD = 0,
	NOT_IMPLEMENTED,
	INVALID_OUTPUT_ARCH,
	INVALID_INSTRUCTION,
	INVALID_LABEL,
	SETUP_CORRUPTED
};

std::ostream& operator<<(std::ostream& ostream, AssemblerError error);

struct AssemblerOptions {
	bool verbose    = false;
	Arch outputArch = Arch::DEFAULT;
};

namespace Assembler {
	struct LabelInfo {
		std::string name;
		size_t lineNumber = 0;
	};

	struct LineInfo {
		std::string sectionName;
		ByteBuffer bytes;
	};

	AssemblerError tokenize(const std::string& str, std::vector<std::vector<std::string>>& tokens);
	AssemblerError assemble(const AssemblerOptions& options, const std::string& fileContent, LCO& lco);
} // namespace Assembler