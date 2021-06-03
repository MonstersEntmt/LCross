#pragma once

#include "Linker/Linker.h"

namespace Formats::Bin {
	LinkerError link(const LinkerOptions& options, ByteBuffer& bytecode);
}