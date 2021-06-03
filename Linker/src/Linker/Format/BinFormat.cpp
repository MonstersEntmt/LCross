#include <Core.h>
#if _TARGETS_BIN_
	#include "Linker/Format/BinFormat.h"

namespace Formats::Bin {
	LinkerError link(const LinkerOptions& options, ByteBuffer& bytecode) {
		Arch outputArch = options.inputFiles[0].outputArch;
		for (auto& lco : options.inputFiles) {
			if (lco.outputArch != outputArch)
				return LinkerError::INVALID_OUTPUT_ARCH;

			for (auto& section : lco.sections)
				bytecode.addBytes(section.bytes);
		}
		return LinkerError::GOOD;
	}
} // namespace Formats::Bin
#endif