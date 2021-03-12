#include "CCompiler/CCompiler.h"

namespace CCompiler {
#ifndef _NO_ASSEMBLER_
	static bool sCompilerNoAsm = false;
	static void handleNoAsmArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
		sCompilerNoAsm = true;
		argUtils.setNoLink();
	}
#endif

	void addInputArgs(ArgUtils& argUtils) {
#ifndef _NO_ASSEMBLER_
		argUtils.addArgInfo("-no_asm", "Do not assemble the file inputs", "This also enables the '-no_link' flag");
		argUtils.addHandler("-no_asm", &handleNoAsmArg);
#endif
	}

	void handleArgs() {

	}

#ifndef _NO_ASSEMBLER_
	bool getNoAsm() {
		return sCompilerNoAsm;
	}
#endif
}