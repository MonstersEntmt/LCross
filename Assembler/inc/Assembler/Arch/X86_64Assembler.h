#pragma once

#include "Assembler/Assembler.h"

namespace Architecture::X86_64 {
	AssemblerError assemble(const AssemblerOptions& options, const std::vector<std::string>& instruction, std::vector<uint8_t>& bytecode);
}