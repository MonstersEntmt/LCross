#pragma once

#include "Linker/Linker.h"

namespace Linker::PE {
	LinkerError link(LinkerState& state, ByteBuffer& bytecode);
}