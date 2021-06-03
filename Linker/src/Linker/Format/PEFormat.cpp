#include <Core.h>
#if _TARGETS_PE_
#include "Linker/Linker.h"

namespace Formats::PE {
	LinkerError link(const LinkerOptions& options, ByteBuffer& bytecode) {
		return LinkerError::NOT_IMPLEMENTED;
	}
}
#endif