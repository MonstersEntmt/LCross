#pragma once

#include "Assembler/Assembler.h"

namespace Architecture::Arm32 {
	AssemblerError assemble(const AssemblerOptions& options, const std::vector<std::string>& instruction, std::vector<uint8_t>& bytecode);
}