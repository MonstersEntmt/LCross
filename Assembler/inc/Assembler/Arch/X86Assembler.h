#pragma once

#include "Assembler/Assembler.h"

namespace Archs::X86 {
	AssemblerError setupAssembler();
	AssemblerError assemble(const AssemblerOptions& options, const std::vector<std::string>& instruction, ByteBuffer& bytes);
} // namespace Archs::X86