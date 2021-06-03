#pragma once

#include "Linker/Linker.h"

namespace Formats::PE {
	LinkerError link(const LinkerOptions& options, ByteBuffer& bytecode);
}