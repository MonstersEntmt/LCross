#pragma once

#include "Common/Format/Format.h"

#include <iostream>
#include <stdint.h>

#define HAS_FLAG(flags, flag) ((flags & flag) == flag)
#define _HAS_ARCH(arch) HAS_FLAG(_ARCH_, arch)
#define _HAS_FORMAT(format) HAS_FLAG(_FORMAT_, format)

#define _ALL UINT32_MAX
#define _X86 1
#define _X86_64 2
#define _ARM32 4
#define _ARM64 8

#define _PE 1
#define _ELF 2
#define _BIN 4

#define _WINDOWS 1
#define _LINUX 2
#define _MACOSX 3

enum class OutputFormat : uint32_t {
	DEFAULT = 0,
	PE      = _PE,
	ELF     = _ELF,
	BIN     = _BIN
};

enum class OutputArch : uint32_t {
	DEFAULT = 0,
	X86     = _X86,
	X86_64  = _X86_64,
	ARM32   = _ARM32,
	ARM64   = _ARM64
};

enum class TargetPlatform : uint32_t {
	DEFAULT = 0,
	WINDOWS = _WINDOWS,
	LINUX   = _LINUX,
	MACOSX  = _MACOSX
};

struct HostInfo {
	TargetPlatform platform;
	OutputArch arch;
};

template <>
struct Format::Formatter<OutputFormat> {
	std::string format(OutputFormat format, const std::string& options) {
		switch (format) {
		case OutputFormat::DEFAULT: return "Default";
		case OutputFormat::PE: return "PE";
		case OutputFormat::ELF: return "ELF";
		case OutputFormat::BIN: return "BIN";
		default: return "Unknown";
		}
	}
};

template <>
struct Format::Formatter<OutputArch> {
	std::string format(OutputArch arch, const std::string& options) {
		switch (arch) {
		case OutputArch::DEFAULT: return "Default";
		case OutputArch::X86: return "X86";
		case OutputArch::X86_64: return "X86_64";
		case OutputArch::ARM32: return "ARM32";
		case OutputArch::ARM64: return "ARM64";
		default: return "Unknown";
		}
	}
};

template <>
struct Format::Formatter<TargetPlatform> {
	std::string format(TargetPlatform platform, const std::string& options) {
		switch (platform) {
		case TargetPlatform::DEFAULT: return "Default";
		case TargetPlatform::WINDOWS: return "Windows";
		case TargetPlatform::LINUX: return "Linux";
		case TargetPlatform::MACOSX: return "Macosx";
		default: return "Unknown";
		}
	}
};

HostInfo getHostInfo();
OutputArch getDefaultCompileArchForHost();
OutputFormat getDefaultCompileFormatForHost();

void printHostPlatform();
void printHostArch();

#if _HAS_ARCH(_X86)
	#define _TARGETS_X86_ true
#else
	#define _TARGETS_X86_ false
#endif

#if _HAS_ARCH(_X86_64)
	#define _TARGETS_X86_64_ true
#else
	#define _TARGETS_X86_64_ false
#endif

#if _HAS_ARCH(_ARM32)
	#define _TARGETS_ARM32_ true
#else
	#define _TARGETS_ARM32_ false
#endif

#if _HAS_ARCH(_ARM64)
	#define _TARGETS_ARM64_ true
#else
	#define _TARGETS_ARM64_ false
#endif

#if _HAS_FORMAT(_PE)
	#define _TARGETS_PE_ true
#else
	#define _TARGETS_PE_ false
#endif

#if _HAS_FORMAT(_ELF)
	#define _TARGETS_ELF_ true
#else
	#define _TARGETS_ELF_ false
#endif

#if _HAS_FORMAT(_BIN)
	#define _TARGETS_BIN_ true
#else
	#define _TARGETS_BIN_ false
#endif

#if !(_TARGETS_X86_ || _TARGETS_X86_64_ || _TARGETS_ARM32_ || _TARGETS_ARM64_)
	#error Given target(s) in _ARCH_ macro is not a supported target!
#endif

#if !(_TARGETS_PE_ || _TARGETS_ELF_ || _TARGETS_BIN_)
	#error Given format(s) in _FORMAT_ macro is not a supported format!
#endif