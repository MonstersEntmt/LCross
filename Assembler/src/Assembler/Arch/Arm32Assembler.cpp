#include <Core.h>
#if _TARGETS_ARM32_
#include "Assembler/Arch/X86_64Assembler.h"

namespace Architecture::Arm32 {
	AssemblerError assemble(const AssemblerOptions& options, const std::vector<std::string>& instruction, std::vector<uint8_t>& bytecode) {
		return AssemblerError::NOT_IMPLEMENTED;
	}
}
#endif