#include "Core.h"

#include "Utils/PrintUtils.h"

HostInfo getHostInfo() {
	return { (Platform) _HOST_PLATFORM_, (Arch) _HOST_ARCH_ };
}

std::ostream& operator<<(std::ostream& ostream, const Format& format) {
	switch (format) {
	case Format::DEFAULT: return ostream << "Default";
	case Format::PE: return ostream << "PE";
	case Format::ELF: return ostream << "ELF";
	case Format::BIN: return ostream << "BIN";
	default: return ostream << "Unknown";
	}
}

std::ostream& operator<<(std::ostream& ostream, const Arch& arch) {
	switch (arch) {
	case Arch::DEFAULT: return ostream << "Default";
	case Arch::X86: return ostream << "X86";
	case Arch::X86_64: return ostream << "X86_64";
	case Arch::ARM32: return ostream << "ARM32";
	case Arch::ARM64: return ostream << "ARM64";
	default: return ostream << "Unknown";
	}
}

std::ostream& operator<<(std::ostream& ostream, const Platform& platform) {
	switch (platform) {
	case Platform::DEFAULT: return ostream << "Default";
	case Platform::WINDOWS: return ostream << "Windows";
	case Platform::LINUX: return ostream << "Linux";
	case Platform::MACOSX: return ostream << "Macosx";
	default: return ostream << "Unknown";
	}
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
	std::cout << PrintUtils::appInfo << "Host platform is " << PrintUtils::colorSchemeNoteLabel << (Platform) _HOST_PLATFORM_ << PrintUtils::normal << std::endl;
}

void printHostArch() {
	std::cout << PrintUtils::appInfo << "Host platform is " << PrintUtils::colorSchemeNoteLabel << (Arch) _HOST_ARCH_ << PrintUtils::normal << std::endl;
}