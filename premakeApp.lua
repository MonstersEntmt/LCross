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

newoption({
	trigger = "no-builtin-linker",
	description = "There will be no builtin linkers in any of the executables apart from llink"
})

newoption({
	trigger = "no-builtin-assembler",
	description = "There will be no builtin assemblers in any of the executables apart from lasm"
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
			MACRO_ARCH_ = MACRO_ARCH_ .. "+"
		end
		MACRO_ARCH_ = MACRO_ARCH_ .. "_X86_64"
		addedArch = true
	end
	if _OPTIONS["arch-arm32"] then
		if (addedArch) then
			MACRO_ARCH_ = MACRO_ARCH_ .. "+"
		end
		MACRO_ARCH_ = MACRO_ARCH_ .. "_ARM32"
		addedArch = true
	end
	if _OPTIONS["arch-arm64"] then
		if (addedArch) then
			MACRO_ARCH_ = MACRO_ARCH_ .. "+"
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
			MACRO_FORMAT_ = MACRO_FORMAT_ .. "+"
		end
		MACRO_FORMAT_ = MACRO_FORMAT_ .. "_ELF"
		addedFormat = true
	end
	if _OPTIONS["format-bin"] then
		if (addedFormat) then
			MACRO_FORMAT_ = MACRO_FORMAT_ .. "+"
		end
		MACRO_FORMAT_ = MACRO_FORMAT_ .. "_BIN"
		addedFormat = true
	end
end

local common = APP.GetOrCreateApp("Common")
common.group = "Libs"
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

local linker = APP.GetOrCreateApp("Linker")
linker.kind = "ConsoleApp"
linker.AddDependency(common)
linker.AddState({}, function()
	targetname("llink")
end)

local linkerLib = APP.GetOrCreateApp("LinkerLib")
linkerLib.location = linker.name .. "/"
linkerLib.includeDir = linker.includeDir
linkerLib.resourceDir = linker.resourceDir
linkerLib.sourceDir = linker.sourceDir
linkerLib.name = "LinkerLib"
linkerLib.group = "Libs"
linkerLib.kind = "StaticLib"
linkerLib.AddDependency(common)
linkerLib.files = {
	linkerLib.currentPath .. linkerLib.includeDir .. "Linker/**.h",
	linkerLib.currentPath .. linkerLib.includeDir .. "Linker/**.hpp",
	linkerLib.currentPath .. linkerLib.sourceDir .. "Linker/**.h",
	linkerLib.currentPath .. linkerLib.sourceDir .. "Linker/**.hpp",
	linkerLib.currentPath .. linkerLib.sourceDir .. "Linker/**.c",
	linkerLib.currentPath .. linkerLib.sourceDir .. "Linker/**.cpp"
}

local assembler = APP.GetOrCreateApp("Assembler")
assembler.kind = "ConsoleApp"
assembler.AddDependency(common)
if not _OPTIONS["no-builtin-linker"] then
	assembler.AddDependency(linkerLib)
end
assembler.AddState({}, function()
	if _OPTIONS["no-builtin-linker"] then
		defines({ "_NO_LINKER_" })
	end
	targetname("lasm")
end)

local assemblerLib = APP.GetOrCreateApp("AssemblerLib")
assemblerLib.location = assembler.name .. "/"
assemblerLib.includeDir = assembler.includeDir
assemblerLib.resourceDir = assembler.resourceDir
assemblerLib.sourceDir = assembler.sourceDir
assemblerLib.group = "Libs"
assemblerLib.kind = "StaticLib"
assemblerLib.files = {
	assemblerLib.currentPath .. assemblerLib.includeDir .. "Assembler/**.h",
	assemblerLib.currentPath .. assemblerLib.includeDir .. "Assembler/**.hpp",
	assemblerLib.currentPath .. assemblerLib.sourceDir .. "Assembler/**.h",
	assemblerLib.currentPath .. assemblerLib.sourceDir .. "Assembler/**.hpp",
	assemblerLib.currentPath .. assemblerLib.sourceDir .. "Assembler/**.c",
	assemblerLib.currentPath .. assemblerLib.sourceDir .. "Assembler/**.cpp"
}
assemblerLib.AddDependency(common)
if not _OPTIONS["no-builtin-linker"] then
	assemblerLib.AddDependency(linkerLib)
end
assemblerLib.AddGlobalState({}, function()
	if _OPTIONS["no-builtin-linker"] then
		defines({ "_NO_LINKER_" })
	end
end)

local cCompiler = APP.GetOrCreateApp("CCompiler")
cCompiler.kind = "ConsoleApp"
cCompiler.AddDependency(common)
if not _OPTIONS["no-builtin-assembler"] then
	cCompiler.AddDependency(assemblerLib)
end
cCompiler.AddState({}, function()
	if _OPTIONS["no-builtin-assembler"] then
		defines({ "_NO_ASSEMBLER_" })
		defines({ "_NO_LINKER_" })
	end
	targetname("lcc")
end)

local cppCompiler = APP.GetOrCreateApp("CppCompiler")
cppCompiler.kind = "ConsoleApp"
cppCompiler.AddDependency(common)
if not _OPTIONS["no-builtin-assembler"] then
	cppCompiler.AddDependency(assemblerLib)
end
cppCompiler.AddState({}, function()
	if _OPTIONS["no-builtin-assembler"] then
		defines({ "_NO_ASSEMBLER_" })
		defines({ "_NO_LINKER_" })
	end
	targetname("lcpp")
end)

return { linker, linkerLib, assembler, assemblerLib, cCompiler, cppCompiler, common }