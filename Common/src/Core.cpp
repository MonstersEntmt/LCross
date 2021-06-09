#include "Core.h"

#include "Common/Logger.h"
#include "Common/PrintUtils.h"

HostInfo getHostInfo() {
	return { (TargetPlatform) _HOST_PLATFORM_, (OutputArch) _HOST_ARCH_ };
}

OutputArch getDefaultCompileArchForHost() {
#if _HOST_ARCH_ == _X86
	#if _TARGETS_X86_
	return OutputArch::X86;
	#elif _TARGETS_X86_64_
	return OutputArch::X86_64;
	#elif _TARGETS_ARM32_
	return OutputArch::ARM32;
	#elif _TARGETS_ARM64_
	return OutputArch::ARM64;
	#endif
#elif _HOST_ARCH_ == _X86_64
	#if _TARGETS_X86_64_
	return OutputArch::X86_64;
	#elif _TARGETS_X86_
	return OutputArch::X86;
	#elif _TARGETS_ARM32_
	return OutputArch::ARM32;
	#elif _TARGETS_ARM64_
	return OutputArch::ARM64;
	#endif
#elif _HOST_ARCH_ == _ARM32
	#if _TARGETS_ARM32_
	return OutputArch::ARM32;
	#elif _TARGETS_ARM64_
	return OutputArch::ARM64;
	#elif _TARGETS_X86_
	return OutputArch::X86;
	#elif _TARGETS_X86_64_
	return OutputArch::X86_64;
	#endif
#elif _HOST_ARCH_ == _ARM64
	#if _TARGETS_ARM64_
	return OutputArch::ARM64;
	#elif _TARGETS_ARM32_
	return OutputArch::ARM32;
	#elif _TARGETS_X86_
	return OutputArch::X86;
	#elif _TARGETS_X86_64_
	return OutputArch::X86_64;
	#endif
#endif
}

OutputFormat getDefaultCompileFormatForHost() {
#if _HOST_PLATFORM_ == _WINDOWS
	#if _TARGETS_PE_
	return OutputFormat::PE;
	#elif _TARGETS_BIN_
	return OutputFormat::BIN;
	#elif _TARGETS_ELF_
	return OutputFormat::ELF;
	#endif
#elif _HOST_PLATFORM_ == _LINUX
	#if _TARGETS_ELF_
	return OutputFormat::ELF;
	#elif _TARGETS_BIN_
	return OutputFormat::BIN;
	#elif _TARGETS_PE_
	return OutputFormat::PE;
	#endif
#elif _HOST_PLATFORM_ == _MACOSX
	#if _TARGETS_ELF_
	return OutputFormat::ELF;
	#elif _TARGETS_BIN_
	return OutputFormat::BIN;
	#elif _TARGETS_PE_
	return OutputFormat::PE;
	#endif
#endif
}

void printHostPlatform() {
	Logger::log(LogSeverity::Info, "Host platform is {}'{}'{}", LogColors::Arg, static_cast<TargetPlatform>(_HOST_PLATFORM_), LogColors::Info);
}

void printHostArch() {
	Logger::log(LogSeverity::Info, "Host arch is {}'{}'{}", LogColors::Arg, static_cast<OutputArch>(_HOST_ARCH_), LogColors::Info);
}