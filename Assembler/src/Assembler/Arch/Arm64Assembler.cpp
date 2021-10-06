#include <Core.h>
#if _TARGETS_ARM64_
	#include "Assembler/Arch/Arm64Assembler.h"

namespace Assembler::Arm64 {
	AssemblerError assemble(AssemblerState& state, LineInfo& lineInfo, const std::vector<std::string>& instruction) {
		return AssemblerError::NOT_IMPLEMENTED;
	}
} // namespace Assembler::Arm64
#endif