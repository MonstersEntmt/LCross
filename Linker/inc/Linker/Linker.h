#pragma once

#include <Utils/ArgUtils.h>

namespace Linker {
	void addInputArgs(ArgUtils& argUtils);
	void handleArgs();
	void handleOutputName(ArgUtils& argUtils, std::string& outputName);
	Format getFormat();
}