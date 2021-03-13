#pragma once

#include <Common/ArgUtils.h>
#include <Core.h>

class LinkerArgUtils : public ArgUtils {
public:
	LinkerArgUtils(int argc, char** argv);

	virtual void handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) override;
	virtual void handleFormatFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed);

	virtual void handleOutputName() override;

	virtual void handleVirt() override;

public: // Passed Arg Data
	Format outputFormat = Format::DEFAULT;
};

enum class LinkerError : uint32_t {
	GOOD = 0,
	NOT_IMPLEMENTED
};

std::ostream& operator<<(std::ostream& ostream, LinkerError linkerError);

struct LinkerOptions {
	bool verbose = false;
	Format outputFormat = Format::DEFAULT;
	std::vector<std::vector<uint8_t>> inputFiles;
};

namespace Linker {
	LinkerError link(const LinkerOptions& options, std::vector<uint8_t>& bytecode);
}