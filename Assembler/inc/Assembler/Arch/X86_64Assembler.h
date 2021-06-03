#pragma once

#include "Assembler/Assembler.h"

namespace Archs::X86_64 {
	AssemblerError assemble(const AssemblerOptions& options, const std::vector<std::string>& instruction, ByteBuffer& bytes);
}