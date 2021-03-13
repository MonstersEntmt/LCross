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
	trigger = "assembler-no-link",
	description = "Forces the Assembler to not include a built in linking"
})

newoption({
	trigger = "c-compiler-no-link",
	description = "Forces the C Compiler to not include a built in linking"
})

newoption({
	trigger = "c-compiler-no-assemble",
	description = "Forces the C Compiler to not include a built in assembler"
})

newoption({
	trigger = "cpp-compiler-no-link",
	description = "Forces the C++ Compiler to not include a built in linking"
})

newoption({
	trigger = "cpp-compiler-no-assemble",
	description = "Forces the C++ Compiler to not include a built in assembler"
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

local assembler = APP.GetOrCreateApp("Assembler")
assembler.kind = "ConsoleApp"
assembler.AddDependency(common)
assembler.AddState({}, function()
	if _OPTIONS["assembler-no-link"] then
		defines({ "_NO_LINKER_" })
	else
		sysincludedirs({ linker.currentPath .. linker.includeDir })
		files({
			linker.currentPath .. linker.includeDir .. "Linker/**.h",
			linker.currentPath .. linker.includeDir .. "Linker/**.hpp",
			linker.currentPath .. linker.sourceDir .. "Linker/**.h",
			linker.currentPath .. linker.sourceDir .. "Linker/**.hpp",
			linker.currentPath .. linker.sourceDir .. "Linker/**.c",
			linker.currentPath .. linker.sourceDir .. "Linker/**.cpp"
		})
	end
	targetname("lasm")
end)

local cCompiler = APP.GetOrCreateApp("CCompiler")
cCompiler.kind = "ConsoleApp"
cCompiler.AddDependency(common)
cCompiler.AddState({}, function()
	if _OPTIONS["c-compiler-no-link"] then
		defines({ "_NO_LINKER_" })
	elseif not _OPTIONS["c-compiler-no-assemble"] then
		sysincludedirs({ linker.currentPath .. linker.includeDir })
		files({
			linker.currentPath .. linker.includeDir .. "Linker/**.h",
			linker.currentPath .. linker.includeDir .. "Linker/**.hpp",
			linker.currentPath .. linker.sourceDir .. "Linker/**.h",
			linker.currentPath .. linker.sourceDir .. "Linker/**.hpp",
			linker.currentPath .. linker.sourceDir .. "Linker/**.c",
			linker.currentPath .. linker.sourceDir .. "Linker/**.cpp"
		})
	end
	if _OPTIONS["c-compiler-no-assemble"] then
		defines({ "_NO_ASSEMBLER_" })
		defines({ "_NO_LINKER_" })
	else
		sysincludedirs({ assembler.currentPath .. assembler.includeDir })
		files({
			assembler.currentPath .. assembler.includeDir .. "Assembler/**.h",
			assembler.currentPath .. assembler.includeDir .. "Assembler/**.hpp",
			assembler.currentPath .. assembler.sourceDir .. "Assembler/**.h",
			assembler.currentPath .. assembler.sourceDir .. "Assembler/**.hpp",
			assembler.currentPath .. assembler.sourceDir .. "Assembler/**.c",
			assembler.currentPath .. assembler.sourceDir .. "Assembler/**.cpp"
		})
	end
	targetname("lcc")
end)

local cppCompiler = APP.GetOrCreateApp("CppCompiler")
cppCompiler.kind = "ConsoleApp"
cppCompiler.AddDependency(common)
cppCompiler.AddState({}, function()
	if _OPTIONS["cpp-compiler-no-link"] then
		defines({ "_NO_LINKER_" })
	elseif not _OPTIONS["cpp-compiler-no-assemble"] then
		sysincludedirs({ linker.currentPath .. linker.includeDir })
		files({
			linker.currentPath .. linker.includeDir .. "Linker/**.h",
			linker.currentPath .. linker.includeDir .. "Linker/**.hpp",
			linker.currentPath .. linker.sourceDir .. "Linker/**.h",
			linker.currentPath .. linker.sourceDir .. "Linker/**.hpp",
			linker.currentPath .. linker.sourceDir .. "Linker/**.c",
			linker.currentPath .. linker.sourceDir .. "Linker/**.cpp"
		})
	end
	if _OPTIONS["cpp-compiler-no-assemble"] then
		defines({ "_NO_ASSEMBLER_" })
		defines({ "_NO_LINKER_" })
	else
		sysincludedirs({ assembler.currentPath .. assembler.includeDir })
		files({
			assembler.currentPath .. assembler.includeDir .. "Assembler/**.h",
			assembler.currentPath .. assembler.includeDir .. "Assembler/**.hpp",
			assembler.currentPath .. assembler.sourceDir .. "Assembler/**.h",
			assembler.currentPath .. assembler.sourceDir .. "Assembler/**.hpp",
			assembler.currentPath .. assembler.sourceDir .. "Assembler/**.c",
			assembler.currentPath .. assembler.sourceDir .. "Assembler/**.cpp"
		})
	end
	targetname("lcpp")
end)

return { assembler, linker, cCompiler, cppCompiler, common }