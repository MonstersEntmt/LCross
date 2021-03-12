# LCross
Lava Cross Compiler is a cross compiler built to support any major host platform<br>
Currently they are:
- Windows
- Linux (Might not work haven't tested)
- Macosx (Might not work haven't tested)
- More to come I guess

And will supports building `pe`, `elf` and `bin` files using `x86`, `x86_64`, `arm32` and `arm64` architectures.<br>
And supports disabling unneeded formats and architectures, all with just some arguments passed to Premake5.

# Roadmap
Currently this project is far from done.<br>
Here's a list of things to come:
- Assembler "LASM"
- Linker "LLINK"
- C Compiler "LCC"
- C++ Compiler "LCPP"

### Q. How to build for given file formats and architectures
A. Give Premake5 one of the following arguments to enable specific file formats or architectures.<br>
Formats (by default all are enabled):
- `--format-pe`: Makes windows `.exe` files
- `--format-elf`: Makes elf `.o` files
- `--format-bin`: Makes binary `.bin` files

Architectures (by default all are enabled):
- `--arch-x86`: Makes x86 compatible files
- `--arch-x86_64`: Makes x86_64 compatible files
- `--arch-arm32`: Makes arm32 compatible files
- `--arch-arm64`: Makes arm64 compatible files

### Q. Can I contribute
A. Yes please :D