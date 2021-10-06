#pragma once

#include <LCross/Core.hpp>

namespace LCross::LLC {
    struct AssemblerState {
        struct Options {
            bool verbose = false;
            Architecture outputArchitecture = Architecture::Default;
        } options;
    };

    struct CompilerState {
        struct Options {
            bool verbose = false;
            Architecture outputArchitecture = Architecture::Default;
        } options;
    };

    void assemble(AssemblerState& state/*, LClass lclass*/);
    void compile(CompilerState& state/*, std::string& assembly*/);
}