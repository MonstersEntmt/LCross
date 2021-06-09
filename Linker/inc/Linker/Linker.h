#pragma once

#include <Common/ArgUtils.h>
#include <Common/ByteBuffer.h>
#include <Common/LCO.h>
#include <Common/Logger.h>
#include <Core.h>

class LinkerArgUtils : public ArgUtils {
public:
	LinkerArgUtils(int argc, char** argv);

	virtual void handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) override;
	virtual void handleFormatFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed);

	virtual void handleOutputName() override;

	virtual void handleVirt() override;

public: // Passed Arg Data
	OutputFormat outputFormat = OutputFormat::DEFAULT;
};

enum class LinkerError : uint32_t {
	GOOD = 0,
	NOT_IMPLEMENTED,
	INVALID_OUTPUT_FORMAT,
	INVALID_OUTPUT_ARCH,
	MISSING_EXTERNAL_SYMBOL,
	MULTIPLY_DEFINED_SYMBOLS
};

template <>
struct Format::Formatter<LinkerError> {
	std::string format(LinkerError error, const std::string& options) {
		switch (error) {
		case LinkerError::GOOD: return "Good";
		case LinkerError::NOT_IMPLEMENTED: return "Linker Not Implemented";
		case LinkerError::INVALID_OUTPUT_FORMAT: return "Invalid Output Format";
		case LinkerError::INVALID_OUTPUT_ARCH: return "Invalid Output Arch";
		case LinkerError::MISSING_EXTERNAL_SYMBOL: return "Missing External Symbol";
		case LinkerError::MULTIPLY_DEFINED_SYMBOLS: return "Multiply Defined Symbols";
		default: return "Unknown";
		}
	}
};
namespace Linker {
	struct LinkerState {
		struct Options {
			bool verbose              = false;
			OutputFormat outputFormat = OutputFormat::DEFAULT;
			std::vector<LCO> inputFiles;
			std::string entryPoint;
		} options;
	};

	LinkerError link(LinkerState& state, ByteBuffer& bytecode);
} // namespace Linker