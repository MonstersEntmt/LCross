#pragma once

#include "Defines.hpp"

namespace LCross {
    enum class Architecture : std::uint32_t {
        Default = 0,
        X86     = LCROSS_SUPPORT_ARCH_X86,
        X86_64  = LCROSS_SUPPORT_ARCH_X86_64,
        ARM32   = LCROSS_SUPPORT_ARCH_ARM32,
        ARM64   = LCROSS_SUPPORT_ARCH_ARM64
    };

    enum class Format : std::uint32_t {
        Default = 0,
        PE      = LCROSS_SUPPORT_FORMAT_PE,
        ELF     = LCROSS_SUPPORT_FORMAT_ELF,
        BIN     = LCROSS_SUPPORT_FORMAT_BIN
    };

    enum class System : std::uint32_t {
        Default = 0,
        Windows = LCROSS_SYSTEM_WINDOWS,
        Unix    = LCROSS_SYSTEM_UNIX,
        Linux   = LCROSS_SYSTEM_LINUX,
        Macos   = LCROSS_SYSTEM_MACOS
    };

    struct HostInformation {
        System       m_System;
        Architecture m_Arch;
    };

    HostInformation getHostInfo();
    Architecture    getDefaultArchitectureForHost();
    Format          getDefaultFormatForHost();
}
