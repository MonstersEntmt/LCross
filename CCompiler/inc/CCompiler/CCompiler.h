#pragma once

#include <Utils/ArgUtils.h>

namespace CCompiler {
	void addInputArgs(ArgUtils& argUtils);
	void handleArgs();
#ifndef _NO_ASSEMBLER_
	bool getNoAsm();
#endif
}