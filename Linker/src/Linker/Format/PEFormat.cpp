#include <Core.h>
#if _TARGETS_PE_
	#include "Linker/Linker.h"

namespace Linker::PE {
	LinkerError link(LinkerState& state, ByteBuffer& bytecode) {
		return LinkerError::NOT_IMPLEMENTED;
	}
} // namespace Linker::PE
#endif