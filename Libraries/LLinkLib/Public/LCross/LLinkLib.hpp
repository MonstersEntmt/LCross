#pragma once

#include <LCross/Core.hpp>
#include <LCross/Common/ByteBuffer.hpp>

namespace LCross::LLink {
	struct LinkerState {
		struct Options {
			bool verbose        = false;
			Format outputFormat = Format::Default;
		} options;
	};

	void link(LinkerState& state, ::LCross::Common::ByteBuffer& byteCode);
} // namespace LCross::LLink
