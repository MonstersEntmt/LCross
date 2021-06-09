#pragma once

#include "Linker/Linker.h"

namespace Linker::Bin {
	LinkerError link(LinkerState& state, ByteBuffer& bytecode);
}