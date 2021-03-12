# LCross
Lava Cross Compiler is a cross compiler built to support any major host platform<br>
Currently they are:
- Windows
- Linux (Might not work haven't tested)
- Macosx (Might not work haven't tested)
- More to come I guess

And will supports linking `pe`, `elf` and `bin` files using `x86`, `x86_64`, `arm32` and `arm64` architectures.<br>
And supports disabling unneeded formats and architectures, all with just some arguments passed to Premake5.

# How to Build
Simply download Premake5 and run `premake5.exe <action>` where `<action>` is one of their supported actions.<br>
If you want LCross to only be able to link `PE` or `Elf` files use one, or more of the following arguments:<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;(Note: By default all format's are enabled)
- `--format-pe`: Makes LCross link pe `.exe` files
- `--format-elf`: Makes LCross link elf `.o` files
- `--format-bin`: Makes LCross link binary `.bin` files

If you want LCross to only be able to compile `x86` or `arm32` files use one, or more of the following arguments:<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;(Note: By default all architectures are enabled)
- `--arch-x86`: Makes LCross compile `x86` compile files
- `--arch-x86_64`: Makes LCross compile `x86_64` compile files
- `--arch-arm32`: Makes LCross compile `arm32` compile files
- `--arch-arm64`: Makes LCross compile `arm64` compile files

Extra arguments:
- `--assembler-no-link`: Disables LASM's ability to link files.
- `--c-compiler-no-link`: Disables LCC's ability to link files.
- `--c-compiler-no-assemble`: Disables LCC's ability to assemble files.
- `--cpp-compiler-no-link`: Disables LCPP's ability to link files.
- `--cpp-compiler-no-assemble`: Disables LCPP's ability to assemble files.

# Roadmap
Currently this project is far from done.<br>
Here's a list of things to come:
- Assembler "LASM"
- Linker "LLINK"
- C Compiler "LCC"
- C++ Compiler "LCPP"

### Q. Can I contribute
A. Yes please :D
