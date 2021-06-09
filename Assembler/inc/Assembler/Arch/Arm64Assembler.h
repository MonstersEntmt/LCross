#pragma once

#include "Assembler/Assembler.h"

namespace Assembler::Arm64 {
	AssemblerError assemble(AssemblerState& state, LineInfo& lineInfo, const std::vector<std::string>& instruction);
}