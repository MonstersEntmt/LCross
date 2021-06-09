#include <Core.h>
#if _TARGETS_ELF_
	#include "Linker/Linker.h"

namespace Linker::ELF {
	enum class ELFType : uint16_t {
		NONE   = 0x0000,
		REL    = 0x0001,
		EXEC   = 0x0002,
		DYN    = 0x0003,
		CORE   = 0x0004,
		LOOS   = 0xFE00,
		HIOS   = 0xFEFF,
		LOPROC = 0xFF00,
		HIPROC = 0xFFFF
	};

	uint8_t getClassFromArch(OutputArch arch) {
		switch (arch) {
		case OutputArch::X86:
		case OutputArch::ARM32:
			return 0x01;
		case OutputArch::X86_64:
		case OutputArch::ARM64:
			return 0x02;
		}
		return 0;
	}

	uint8_t getDataFromArch(OutputArch arch) {
		switch (arch) {
		case OutputArch::X86:
		case OutputArch::X86_64:
			return 0x01;
		case OutputArch::ARM32:
		case OutputArch::ARM64:
			return 0x01; // This could be 0x02 as well
		}
		return 0;
	}

	uint16_t getMachineFromArch(OutputArch arch) {
		switch (arch) {
		case OutputArch::X86:
			return 0x0003;
		case OutputArch::X86_64:
			return 0x003E;
		case OutputArch::ARM32:
			return 0x0028;
		case OutputArch::ARM64:
			return 0x00B7;
		}
		return 0;
	}

	struct ELFProgramHeaderEntry {
		uint32_t type;
		uint32_t flags;
		uint64_t offset;
		uint64_t vaddr;
		uint64_t paddr;
		uint64_t filesz;
		uint64_t memsz;
		uint64_t align;
	};

	struct ELFSectionHeaderEntry {
		uint32_t name;
		uint32_t type;
		uint64_t flags;
		uint64_t addr;
		uint64_t offset;
		uint64_t size;
		uint32_t link;
		uint32_t info;
		uint64_t addralign;
		uint64_t entsize;
	};

	LinkerError link(LinkerState& state, ByteBuffer& bytecode) {
		uint64_t entryPoint                   = 0x0000000000000000;
		uint64_t sectionHeaderAddr            = 0x0000000000000000;
		uint16_t programHeaderTableEntryCount = 0x0000;
		uint16_t sectionHeaderTableEntryCount = 0x0000;
		uint16_t sectionNameIndex             = 0x0000;
		std::vector<ELFProgramHeaderEntry> programHeaderEntries;
		std::vector<ELFSectionHeaderEntry> sectionHeaderEntries;

		OutputArch arch  = OutputArch::X86_64; // TODO: Get arch from input file
		uint8_t EI_CLASS = getClassFromArch(arch);
		// ELF Header
		bytecode.addUInt8(0x7F); // E_Ident[EI_Mag]
		bytecode.addStringNNT("ELF");
		bytecode.addUInt8(EI_CLASS);                  // E_Ident[EI_Class]
		bytecode.addUInt8(getDataFromArch(arch));     // E_Ident[EI_Data]
		bytecode.addUInt8(0x01);                      // E_Ident[EI_Version]
		bytecode.addUInt8(0x00);                      // E_Ident[EI_OsABI]
		bytecode.addUInt64(0x0000000000000000ULL);    // E_Ident[EI_ABIVersion + EI_Pad]
		bytecode.addUInt16((uint16_t) ELFType::EXEC); // E_Type (TODO: Add support for linking dynamic libs as well)
		bytecode.addUInt16(getMachineFromArch(arch)); // E_Machine
		bytecode.addUInt32(0x00000001);               // E_Version
		if (EI_CLASS == 0x01) {
			bytecode.addUInt32((uint32_t) entryPoint);        // E_Entry 32-bit
			bytecode.addUInt32(0x00000034);                   // E_PhOff 32-bit
			bytecode.addUInt32((uint32_t) sectionHeaderAddr); // E_ShOff 32-bit
		} else if (EI_CLASS == 0x02) {
			bytecode.addUInt64(entryPoint);        // E_Entry 64-bit
			bytecode.addUInt64(0x00000040);        // E_PhOff 64-bit
			bytecode.addUInt64(sectionHeaderAddr); // E_ShOff 64-bit
		}
		bytecode.addUInt32(0x00000000); // E_Flags
		bytecode.addUInt16(EI_CLASS == 0x01 ? 0x0034 : EI_CLASS == 0x02 ? 0x0040 :
                                                                          0x0000); // E_EhSize
		bytecode.addUInt16(EI_CLASS == 0x01 ? 0x0020 : EI_CLASS == 0x02 ? 0x0038 :
                                                                          0x0000); // E_PhEntSize
		bytecode.addUInt16(programHeaderTableEntryCount);                          // E_PhNum
		bytecode.addUInt16(EI_CLASS == 0x01 ? 0x0028 : EI_CLASS == 0x02 ? 0x0040 :
                                                                          0x0000); // E_ShEntSize
		bytecode.addUInt16(sectionHeaderTableEntryCount);                          // E_ShNum
		bytecode.addUInt16(sectionNameIndex);                                      // E_ShStrNdx
		// Program Header Table
		for (ELFProgramHeaderEntry& entry : programHeaderEntries) {
			bytecode.addUInt32(entry.type);
			if (EI_CLASS == 0x01) {
				bytecode.addUInt64((uint32_t) entry.offset); // P_Offset 32-bit
				bytecode.addUInt64((uint32_t) entry.vaddr);  // P_VAddr 32-bit
				bytecode.addUInt64((uint32_t) entry.paddr);  // P_PAddr 32-bit
				bytecode.addUInt64((uint32_t) entry.filesz); // P_FileSz 32-bit
				bytecode.addUInt64((uint32_t) entry.memsz);  // P_MemSz 32-bit
				bytecode.addUInt32(entry.flags);             // P_Flags 32-bit
				bytecode.addUInt64((uint32_t) entry.align);  // P_Align 32-bit
			} else if (EI_CLASS == 0x02) {
				bytecode.addUInt32(entry.flags);  // P_Flags 64-bit
				bytecode.addUInt64(entry.offset); // P_Offset 64-bit
				bytecode.addUInt64(entry.vaddr);  // P_VAddr 64-bit
				bytecode.addUInt64(entry.paddr);  // P_PAddr 64-bit
				bytecode.addUInt64(entry.filesz); // P_FileSz 64-bit
				bytecode.addUInt64(entry.memsz);  // P_MemSz 64-bit
				bytecode.addUInt64(entry.align);  // P_Align 64-bit
			}
		}
		// Code
		// Data
		// Section Names
		// Section Header Table (Ignored for execution)
		for (ELFSectionHeaderEntry& entry : sectionHeaderEntries) {
			bytecode.addUInt32(entry.name); // Sh_Name
			bytecode.addUInt32(entry.type); // Sh_Type
			if (EI_CLASS == 0x01) {
				bytecode.addUInt32((uint32_t) entry.flags);     // Sh_Flags 32-bit
				bytecode.addUInt32((uint32_t) entry.addr);      // Sh_Addr 32-bit
				bytecode.addUInt32((uint32_t) entry.offset);    // Sh_Offset 32-bit
				bytecode.addUInt32((uint32_t) entry.size);      // Sh_Size 32-bit
				bytecode.addUInt32(entry.link);                 // Sh_Link
				bytecode.addUInt32(entry.info);                 // Sh_Info
				bytecode.addUInt32((uint32_t) entry.addralign); // Sh_AddrAlign 32-bit
				bytecode.addUInt32((uint32_t) entry.entsize);   // Sh_EntSize 32-bit
			} else if (EI_CLASS == 0x02) {
				bytecode.addUInt64(entry.flags);     // Sh_Flags 64-bit
				bytecode.addUInt64(entry.addr);      // Sh_Addr 64-bit
				bytecode.addUInt64(entry.offset);    // Sh_Offset 64-bit
				bytecode.addUInt64(entry.size);      // Sh_Size 64-bit
				bytecode.addUInt64(entry.addralign); // Sh_AddrAlign 64-bit
				bytecode.addUInt64(entry.entsize);   // Sh_EntSize 64-bit
			}
		}

		return LinkerError::GOOD;
	}
} // namespace Linker::ELF
#endif