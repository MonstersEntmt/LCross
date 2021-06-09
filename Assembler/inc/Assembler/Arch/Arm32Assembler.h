#pragma once

#include "Assembler/Assembler.h"

namespace Assembler::Arm32 {
	AssemblerError assemble(AssemblerState& state, LineInfo& lineInfo, const std::vector<std::string>& instruction);
}