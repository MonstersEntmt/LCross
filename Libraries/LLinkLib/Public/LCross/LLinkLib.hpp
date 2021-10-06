#pragma once

#include <LCross/Core.hpp>

namespace LCross::LLink {
    struct LinkerState {
        struct Options {
            bool verbose = false;
            Format outputFormat = Format::Default;
        } options;
    };

    void link(LinkerState& state/*, ByteBuffer& byteCode*/);
}
