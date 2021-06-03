#pragma once

#include "Linker/Linker.h"

namespace Formats::ELF {
	LinkerError link(const LinkerOptions& options, ByteBuffer& bytecode);
}