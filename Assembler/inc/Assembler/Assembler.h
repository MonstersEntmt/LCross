#pragma once

#include <Common/ArgUtils.h>
#include <Common/LCO.h>
#include <Common/Logger.h>
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
	OutputArch outputArch = OutputArch::DEFAULT;
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
	INVALID_LABEL_REFERENCE,
	MULTIPLY_DEFINED_LABELS,
	SETUP_CORRUPTED
};

template <>
struct Format::Formatter<AssemblerError> {
	std::string format(AssemblerError error, const std::string& options) {
		switch (error) {
		case AssemblerError::GOOD: return "Good";
		case AssemblerError::NOT_IMPLEMENTED: return "Assembler Not Implemented";
		case AssemblerError::INVALID_OUTPUT_ARCH: return "Invalid Output Architecture";
		case AssemblerError::INVALID_INSTRUCTION: return "Invalid Instruction";
		case AssemblerError::INVALID_LABEL: return "Invalid Label";
		case AssemblerError::INVALID_LABEL_REFERENCE: return "Invalid Label Reference";
		case AssemblerError::MULTIPLY_DEFINED_LABELS: return "Multiply Defined Labels";
		case AssemblerError::SETUP_CORRUPTED: return "Setup Corrupted";
		default: return "Unknown";
		}
	}
};

namespace Assembler {
	struct LabelInfo {
		std::string name;
		uint64_t lineNumber = 0;
		std::string filename;
		uint64_t fileLineNumber = 0;
		uint64_t originAddress  = 0;
		SymbolState state       = SymbolState::HIDDEN;
	};

	struct LabelReference {
		std::string labelName;
		uint64_t byteOffset = 0;
		uint16_t bits       = 0;
	};

	struct LineInfo {
		std::string sectionName;
		std::vector<LabelReference> labelReferences;
		ByteBuffer bytes;
		uint64_t sectionOffset = 0;
		std::string filename;
		uint64_t fileLineNumber = 0;
	};

	struct AssemblerState {
		struct Options {
			bool verbose          = false;
			OutputArch outputArch = OutputArch::DEFAULT;
		} options;
		std::vector<LabelInfo> labelInfos;
		std::vector<LineInfo> lineInfos;
		std::string currentSectionName = ".text";
		std::string currentFileName;
		uint64_t currentLine = 1;
		std::string currentLineText;
		uint16_t maxBits       = 32;
		uint16_t bits          = 32;
		uint64_t originAddress = 0;
	};

	AssemblerError assemble(AssemblerState& state, const std::string& fileContent, LCO& lco);
} // namespace Assembler