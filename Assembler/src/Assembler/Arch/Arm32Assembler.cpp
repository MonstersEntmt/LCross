#include <Core.h>
#if _TARGETS_ARM32_
	#include "Assembler/Arch/Arm32Assembler.h"

namespace Assembler::Arm32 {
	AssemblerError assemble(AssemblerState& state, LineInfo& lineInfo, const std::vector<std::string>& instruction) {
		return AssemblerError::NOT_IMPLEMENTED;
	}
} // namespace Assembler::Arm32
#endif