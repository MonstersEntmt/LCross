#pragma once

#include <LCross/Common/LClass.hpp>
#include <LCross/Core.hpp>

#include <string>

namespace LCross::LLC {
	struct AssemblerState {
		struct Options {
			bool verbose                    = false;
			Architecture outputArchitecture = Architecture::Default;
		} options;
	};

	struct CompilerState {
		struct Options {
			bool verbose                    = false;
			Architecture outputArchitecture = Architecture::Default;
		} options;
	};

	void assemble(AssemblerState& state, LClass& lclass);
	void compile(CompilerState& state, std::string& assembly);
} // namespace LCross::LLC
