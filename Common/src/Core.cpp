#include "Core.h"

#include "Utils/PrintUtils.h"

HostInfo getHostInfo() {
	return { (Platform) _HOST_PLATFORM_, (Arch) _HOST_ARCH_ };
}

Arch getDefaultCompileArchForHost() {
#if _HOST_ARCH_ == _X86
#if _TARGETS_X86_
	return Arch::X86;
#elif _TARGETS_X86_64_
	return Arch::X86_64;
#elif _TARGETS_ARM32_
	return Arch::ARM32;
#elif _TARGETS_ARM64_
	return Arch::ARM64;
#endif
#elif _HOST_ARCH_ == _X86_64
#if _TARGETS_X86_64_
	return Arch::X86_64;
#elif _TARGETS_X86_
	return Arch::X86;
#elif _TARGETS_ARM32_
	return Arch::ARM32;
#elif _TARGETS_ARM64_
	return Arch::ARM64;
#endif
#elif _HOST_ARCH_ == _ARM32
#if _TARGETS_ARM32_
	return Arch::ARM32;
#elif _TARGETS_ARM64_
	return Arch::ARM64;
#elif _TARGETS_X86_
	return Arch::X86;
#elif _TARGETS_X86_64_
	return Arch::X86_64;
#endif
#elif _HOST_ARCH_ == _ARM64
#if _TARGETS_ARM64_
	return Arch::ARM64;
#elif _TARGETS_ARM32_
	return Arch::ARM32;
#elif _TARGETS_X86_
	return Arch::X86;
#elif _TARGETS_X86_64_
	return Arch::X86_64;
#endif
#endif
}

Format getDefaultCompileFormatForHost() {
#if _HOST_PLATFORM_ == _WINDOWS
#if _TARGETS_PE_
	return Format::PE;
#elif _TARGETS_BIN_
	return Format::BIN;
#elif _TARGETS_ELF_
	return Format::ELF;
#endif
#elif _HOST_PLATFORM_ == _LINUX
#if _TARGETS_ELF_
	return Format::ELF;
#elif _TARGETS_BIN_
	return Format::BIN;
#elif _TARGETS_PE_
	return Format::PE;
#endif
#elif _HOST_PLATFORM_ == _MACOSX
#if _TARGETS_ELF_
	return Format::ELF;
#elif _TARGETS_BIN_
	return Format::BIN;
#elif _TARGETS_PE_
	return Format::PE;
#endif
#endif
}

void printHostPlatform() {
	std::cout << PrintUtils::appInfo << "Host platform is " << PrintUtils::colorSchemeNoteLabel;
#if _HOST_PLATFORM_ == _WINDOWS
	std::cout << "Windows";
#elif _HOST_PLATFORM_ == _LINUX
	std::cout << "Linux";
#elif _HOST_PLATFORM_ == _MACOSX
	std::cout << "Macosx";
#endif
	std::cout << PrintUtils::normal << std::endl;
}

void printHostArch() {
	std::cout << PrintUtils::appInfo << "Host platform is " << PrintUtils::colorSchemeNoteLabel;
#if _HOST_ARCH_ == _X86
	std::cout << "x86";
#elif _HOST_ARCH_ == _X86_64
	std::cout << "x86_64";
#elif _HOST_ARCH_ == _ARM32
	std::cout << "arm32";
#elif _HOST_ARCH_ == _ARM64
	std::cout << "arm64";
#endif
	std::cout << PrintUtils::normal << std::endl;
}