#include "LCross/Core.hpp"

namespace LCross {
	HostInformation getHostInfo() {
		return {
			static_cast<System>(LCROSS_SYSTEM),
			static_cast<Architecture>(LCROSS_PLATFORM)
		};
	}

	Architecture getDefaultArchitectureForHost() {
#if LCROSS_PLATFORM_IS_X86
	#if LCROSS_SUPPORTS_ARCH_X86
		return Architecture::X86;
	#elif LCROSS_SUPPORTS_ARCH_X86_64
		return Architecture::X86_64;
	#elif LCROSS_SUPPORTS_ARCH_ARM32
		return Architecture::ARM32;
	#elif LCROSS_SUPPORTS_ARCH_ARM46
		return Architecture::ARM64;
	#endif
#elif LCROSS_PLATFORM_IS_X86_64
	#if LCROSS_SUPPORTS_ARCH_X86_64
		return Architecture::X86_64;
	#elif LCROSS_SUPPORTS_ARCH_X86
		return Architecture::X86;
	#elif LCROSS_SUPPORTS_ARCH_ARM46
		return Architecture::ARM64;
	#elif LCROSS_SUPPORTS_ARCH_ARM32
		return Architecture::ARM32;
	#endif
#endif
	}

	Format getDefaultFormatForHost() {
#if LCROSS_SYSTEM_IS_WINDOWS
	#if LCROSS_SUPPORTS_FORMAT_PE
		return Format::PE;
	#elif LCROSS_SUPPORTS_FORMAT_BIN
		return Format::BIN;
	#elif LCROSS_SUPPORTS_FORMAT_ELF
		return Format::ELF;
	#endif
#elif LCROSS_SYSTEM_IS_UNIX
	#if LCROSS_SUPPORTS_FORMAT_ELF
		return Format::ELF;
	#elif LCROSS_SUPPORTS_FORMAT_BIN
		return Format::BIN;
	#elif LCROSS_SUPPORTS_FORMAT_PE
		return Format::PE;
	#endif
#endif
	}
} // namespace LCross
