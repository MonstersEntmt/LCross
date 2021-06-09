#pragma once

#include "Assembler/Assembler.h"

namespace Assembler::X86 {
	AssemblerError setupAssembler();
	AssemblerError assemble(AssemblerState& state, LineInfo& lineInfo, const std::vector<std::string>& instruction);
} // namespace Archs::X86