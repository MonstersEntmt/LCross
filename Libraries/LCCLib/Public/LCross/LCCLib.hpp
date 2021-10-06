#pragma once

#include <LCross/Core.hpp>

namespace LCross::LCC {
    struct CompilerState {
        struct Options {
            bool verbose = false;
            Architecture outputArchitecture = Architecture::Default;
        } options;
    };

    void compile(CompilerState& state/*, std::string& assembly*/);
}
