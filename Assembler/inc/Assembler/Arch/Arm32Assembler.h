#pragma once

#include "Assembler/Assembler.h"

namespace Archs::Arm32 {
	AssemblerError assemble(const AssemblerOptions& options, const std::vector<std::string>& instruction, ByteBuffer& bytes);
}