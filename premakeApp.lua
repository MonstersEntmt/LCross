newoption({
	trigger = "arch-x86",
	description = "Build for x86 architecture"
})

newoption({
	trigger = "arch-x86_64",
	description = "Build for x86_64 architecture"
})

newoption({
	trigger = "arch-arm32",
	description = "Build for arm32 architecture"
})

newoption({
	trigger = "arch-arm64",
	description = "Build for arm64 architecture"
})

newoption({
	trigger = "format-pe",
	description = "Build for pe file format"
})

newoption({
	trigger = "format-elf",
	description = "Build for elf file format"
})

newoption({
	trigger = "format-bin",
	description = "Build for bin file format"
})

local MACRO_ARCH_ = "_ALL"
local MACRO_FORMAT_ = "_ALL"

if _OPTIONS["arch-x86"] or _OPTIONS["arch-x86_64"] or _OPTIONS["arch-arm32"] or _OPTIONS["arch-arm64"] then
	MACRO_ARCH_ = ""
	local addedArch = false
	if _OPTIONS["arch-x86"] then
		MACRO_ARCH_ = MACRO_ARCH_ .. "_X86"
		addedArch = true
	end
	if _OPTIONS["arch-x86_64"] then
		if (addedArch) then
			MACRO_ARCH_ = MACRO_ARCH_ .. "|"
		end
		MACRO_ARCH_ = MACRO_ARCH_ .. "_X86_64"
		addedArch = true
	end
	if _OPTIONS["arch-arm32"] then
		if (addedArch) then
			MACRO_ARCH_ = MACRO_ARCH_ .. "|"
		end
		MACRO_ARCH_ = MACRO_ARCH_ .. "_ARM32"
		addedArch = true
	end
	if _OPTIONS["arch-arm64"] then
		if (addedArch) then
			MACRO_ARCH_ = MACRO_ARCH_ .. "|"
		end
		MACRO_ARCH_ = MACRO_ARCH_ .. "_ARM64"
		addedArch = true
	end
end

if _OPTIONS["format-pe"] or _OPTIONS["format-elf"] or _OPTIONS["format-bin"] then
	MACRO_FORMAT_ = ""
	local addedFormat = false
	if _OPTIONS["format-pe"] then
		MACRO_FORMAT_ = MACRO_FORMAT_ .. "_PE"
		addedFormat = true
	end
	if _OPTIONS["format-elf"] then
		if (addedFormat) then
			MACRO_FORMAT_ = MACRO_FORMAT_ .. "|"
		end
		MACRO_FORMAT_ = MACRO_FORMAT_ .. "_ELF"
		addedFormat = true
	end
	if _OPTIONS["format-bin"] then
		if (addedFormat) then
			MACRO_FORMAT_ = MACRO_FORMAT_ .. "|"
		end
		MACRO_FORMAT_ = MACRO_FORMAT_ .. "_BIN"
		addedFormat = true
	end
end

local common = APP.GetOrCreateApp("Common")
common.kind = "StaticLib"

common.AddGlobalState({}, function()
	defines({ "_ARCH_=" .. MACRO_ARCH_, "_FORMAT_=" .. MACRO_FORMAT_ })
end)
common.AddGlobalState("system:windows", function()
	defines({ "_HOST_PLATFORM_=_WINDOWS" })
end)
common.AddGlobalState("system:linux", function()
	defines({ "_HOST_PLATFORM_=_LINUX" })
end)
common.AddGlobalState("system:macosx", function()
	defines({ "_HOST_PLATFORM_=_MACOSX" })
end)
common.AddGlobalState("architecture:x86", function()
	defines({ "_HOST_ARCH_=_X86" })
end)
common.AddGlobalState("architecture:x86_64", function()
	defines({ "_HOST_ARCH_=_X86_64" })
end)
common.AddGlobalState("architecture:arm", function()
	defines({ "_HOST_ARCH_=_ARM32" })
end)
common.AddGlobalState("architecture:arm64", function()
	defines({ "_HOST_ARCH_=_ARM64" })
end)

local assembler = APP.GetOrCreateApp("Assembler")
assembler.kind = "ConsoleApp"

assembler.AddDependency(common)

return { assembler, common }