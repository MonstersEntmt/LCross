#pragma once

#include <LCross/Core.hpp>

namespace LCross::LAsm {
	struct AssemblerState {
		struct Options {
			bool verbose                    = false;
			Architecture outputArchitecture = Architecture::Default;
		} options;
	};

	void assemble(AssemblerState& state /*, LCO& lco*/);
} // namespace LCross::LAsm
