newoption({
    trigger = "arch-x86",
    description = "Support the x86 architecture"
})

newoption({
    trigger = "arch-x86_64",
    description = "Support the x86_64 architecture"
})

newoption({
    trigger = "arch-arm32",
    description = "Support the ARM32 architecture"
})

newoption({
    trigger = "arch-arm64",
    description = "Support the ARM64 architecture"
})

newoption({
    trigger = "format-pe",
    description = "Support the PE file format"
})

newoption({
    trigger = "format-elf",
    description = "Support the ELF file format"
})

newoption({
    trigger = "format-bin",
    description = "Support the BIN file format (No header, no junk)"
})

newoption({
    trigger = "no-builtin-linker",
    description = "Don't add a builtin linker in any executables apart from LLink itself"
})

newoption({
    trigger = "no-builtin-assembler",
    description = "Don't add a builtin assembler in any executables apart from LAsm itself"
})

local archMacro = "LCROSS_SUPPORT_ARCH_ALL"
local formatMacro = "LCROSS_SUPPORT_FORMAT_ALL"

if _OPTIONS["arch-x86"] or _OPTIONS["arch-x86_64"] or _OPTIONS["arch-arm32"] or _OPTIONS["arch-arm64"] then
    archMacro = ""
    if _OPTIONS["arch-x86"] then
        archMacro = "LCROSS_SUPPORT_ARCH_X86"
    end
    if _OPTIONS["arch-x86_64"] then
        if archMacro:len() > 0 then
            archMacro = archMacro .. " | "
        end
        archMacro = archMacro .. "LCROSS_SUPPORT_ARCH_X86_64"
    end
    if _OPTIONS["arch-arm32"] then
        if archMacro:len() > 0 then
            archMacro = archMacro .. " | "
        end
        archMacro = archMacro .. "LCROSS_SUPPORT_ARCH_ARM32"
    end
    if _OPTIONS["arch-arm64"] then
        if archMacro:len() > 0 then
            archMacro = archMacro .. " | "
        end
        archMacro = archMacro .. "LCROSS_SUPPORT_ARCH_ARM64"
    end
end

if _OPTIONS["format-pe"] or _OPTIONS["format-elf"] or _OPTIONS["format-bin"] then
    formatMacro = ""
    if _OPTIONS["format-pe"] then
        formatMacro = "LCROSS_SUPPORT_FORMAT_PE"
    end
    if _OPTIONS["format-elf"] then
        if archMacro:len() > 0 then
            formatMacro = formatMacro .. " | "
        end
        formatMacro = formatMacro .. "LCROSS_SUPPORT_FORMAT_LF"
    end
    if _OPTIONS["format-bin"] then
        if formatMacro:len() > 0 then
            formatMacro = formatMacro .. " | "
        end
        formatMacro = formatMacro .. "LCROSS_SUPPORT_FORMAT_BIN"
    end
end

workspace("LCross")
    configurations({ "Debug", "Release", "Dist" })
    platforms({ "x64" })
    
    cppdialect("C++20")
    rtti("Off")
    exceptionhandling("On")
    flags("MultiProcessorCompile")
    
    defines({
        "LCROSS_SUPPORT_ARCH=" .. archMacro,
        "LCROSS_SUPPORT_FORMAT=" .. formatMacro
    })

    filter("configurations:Debug")
        defines({ "LCROSS_CONFIG=LCROSS_CONFIG_DEBUG" })
        optimize("Off")
        symbols("On")
        
    filter("configurations:Release")
        defines({ "LCROSS_CONFIG=LCROSS_CONFIG_RELEASE" })
        optimize("Full")
        symbols("On")
        
    filter("configurations:Dist")
        defines({ "LCROSS_CONFIG=LCROSS_CONFIG_DIST" })
        optimize("Full")
        symbols("Off")
    
    filter("system:windows")
        defines({ "LCROSS_SYSTEM=LCROSS_SYSTEM_WINDOWS" })
    
    filter("system:linux")
        defines({ "LCROSS_SYSTEM=LCROSS_SYSTEM_LINUX" })
    
    filter("system:macosx")
        defines({ "LCROSS_SYSTEM=LCROSS_SYSTEM_MACOS" })
    
    filter("toolset:msc")
        defines({ "LCROSS_TOOLSET=LCROSS_TOOLSET_MSVC" })
        
    filter("toolset:clang")
        defines({ "LCROSS_TOOLSET=LCROSS_TOOLSET_CLANG" })

    filter("toolset:gcc")
        defines({ "LCROSS_TOOLSET=LCROSS_TOOLSET_GCC" })
        
    filter("platforms:x64")
        defines({ "LCROSS_PLATFORM=LCROSS_PLATFORM_X86_64" })
        
    filter({})
    
    startproject("LLink")
    
    group("Libs")
    project("Common")
        kind("StaticLib")
        location("%{wks.location}/Libraries/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })
        
    project("LLinkLib")
        kind("StaticLib")
        location("%{wks.location}/Libraries/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        
        links({ "Common" })
        sysincludedirs({ "%{wks.location}/Libraries/Common/Public/" })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })

    project("LAsmLib")
        kind("StaticLib")
        location("%{wks.location}/Libraries/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        
        links({ "Common" })
        sysincludedirs({ "%{wks.location}/Libraries/Common/Public/" })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })
    
    project("LCCLib")
        kind("StaticLib")
        location("%{wks.location}/Libraries/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        
        links({ "Common" })
        sysincludedirs({ "%{wks.location}/Libraries/Common/Public/" })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })
    
    project("LCPPLib")
        kind("StaticLib")
        location("%{wks.location}/Libraries/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        
        links({ "Common" })
        sysincludedirs({ "%{wks.location}/Libraries/Common/Public/" })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })
    
    project("LLCLib")
        kind("StaticLib")
        location("%{wks.location}/Libraries/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        
        links({ "Common" })
        sysincludedirs({ "%{wks.location}/Libraries/Common/Public/" })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })

    group("Apps")
    project("LLink")
        kind("ConsoleApp")
        location("%{wks.location}/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        debugdir("%{prj.location}/Run/")
        
        links({ "LLinkLib" })
        sysincludedirs({
            "%{wks.location}/Libraries/Common/Public/",
            "%{wks.location}/Libraries/LLinkLib/Public/"
        })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })

    project("LAsm")
        kind("ConsoleApp")
        location("%{wks.location}/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        debugdir("%{prj.location}/Run/")
        
        links({ "LAsmLib" })
        sysincludedirs({
            "%{wks.location}/Libraries/Common/Public/",
            "%{wks.location}/Libraries/LAsmLib/Public/"
        })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })
        
    project("LCC")
        kind("ConsoleApp")
        location("%{wks.location}/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        debugdir("%{prj.location}/Run/")
        
        links({ "LCCLib" })
        sysincludedirs({
            "%{wks.location}/Libraries/Common/Public/",
            "%{wks.location}/Libraries/LCCLib/Public/"
        })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })
        
    project("LCPP")
        kind("ConsoleApp")
        location("%{wks.location}/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        debugdir("%{prj.location}/Run/")
        
        links({ "LCPPLib" })
        sysincludedirs({
            "%{wks.location}/Libraries/Common/Public/",
            "%{wks.location}/Libraries/LCPPLib/Public/"
        })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })
        
    project("LLC")
        kind("ConsoleApp")
        location("%{wks.location}/%{prj.name}/")
        targetdir("%{wks.location}/Bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/")
        objdir("%{wks.location}/Bin/Int-%{cfg.system}-%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}/")
        debugdir("%{prj.location}/Run/")
        
        links({ "LLCLib" })
        sysincludedirs({
            "%{wks.location}/Libraries/Common/Public/",
            "%{wks.location}/Libraries/LLCLib/Public/"
        })
        
        includedirs({
            "%{prj.location}/Private/",
            "%{prj.location}/Public/"
        })
        
        files({ "%{prj.location}/**" })
