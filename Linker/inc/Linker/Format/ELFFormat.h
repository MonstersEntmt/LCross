#pragma once

#include "Linker/Linker.h"

namespace Linker::ELF {
	LinkerError link(LinkerState& state, ByteBuffer& bytecode);
}