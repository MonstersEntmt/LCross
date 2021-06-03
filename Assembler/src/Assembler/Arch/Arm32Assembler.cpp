#include <Core.h>
#if _TARGETS_ARM32_
#include "Assembler/Arch/X86_64Assembler.h"

namespace Archs::Arm32 {
	AssemblerError assemble(const AssemblerOptions& options, const std::vector<std::string>& instruction, ByteBuffer& bytes) {
		return AssemblerError::NOT_IMPLEMENTED;
	}
}
#endif