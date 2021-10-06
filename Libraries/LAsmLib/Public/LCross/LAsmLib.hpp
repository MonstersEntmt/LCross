#pragma once

#include <LCross/Common/LCO.hpp>
#include <LCross/Core.hpp>

namespace LCross::LAsm {
	struct AssemblerState {
		struct Options {
			bool verbose                    = false;
			Architecture outputArchitecture = Architecture::Default;
		} options;
	};

	void assemble(AssemblerState& state, LCO::LCO& lco);
} // namespace LCross::LAsm
