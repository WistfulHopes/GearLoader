# GearLoader - Mod Loader for GGXXACPR (Steam)
Based on the original [Gear-DLL-Loader](https://github.com/MoonSquaredd/Gear-DLL-Loader) by [lovenus](https://github.com/MoonSquaredd)


Version: 0.1.0

GearLoader is a DLL-based mod loader and manager for the Steam release of Guilty Gear Accent Core Plus R.


## Instructions
The DBGHELP.DLL must be placed in the same folder as GGXXACPR_Win.exe 
and any mod that requires it should be placed under a Mods folder 
that you must create yourself (that also has to be in the same folder 
as GGXXACPR_Win.exe, NOT inside Resource).

When playing on Linux/Steam Deck, add `WINEDLLOVERRIDES="dbghelp=n,b" %command%` to the game's launch options.

Additional launch options include:
* `-GearLoaderVerbose` for verbose logging to `GearLoader.log`
* `-DebugConsole` to open a console window on launch


### Modding info
The mod loader can interface with C/C++ DLLs. Each mod should be contained in its own subfolder in the mods folder. Each mod must have a config.json file (see [example mods](https://github.com/YouKnow232/GearLoader/blob/main/examples) and [schema](https://github.com/YouKnow232/GearLoader/blob/main/schema/0.1.0/config.schema.json)).

Each mod DLL may export an `Init` function (see [example mods](https://github.com/YouKnow232/GearLoader/blob/main/examples)). It is advised to use this for startup logic, but it is not required to be loaded. `gearLoader_c.h` is required when using a mod Init function. `gearLoader.hpp` is an optional C++ wrapper (see [example mod](https://github.com/YouKnow232/GearLoader/blob/main/examples/cpp_mod/example/cpp_mod.cpp)).

`ggxxacpr_c.h` and it's C++ wrapper `ggxxacpr.hpp` can be used to interface with game data.

This mod loader uses a dependency and API registry system for inter-mod communication. Dependencies can be declared in a mod's config.json file, and their APIs can be retrieved with `GearLoaderApi::RetrieveModApi`. The mod loader includes a base mod that exports a modding API.

When using the base mod's modding API, both `baseMod_c.h` and `ggxxacpr_c.h` are required. Both have optional C++ header wrappers as well.


## Build Instructions
### Prerequisites
* CMake v3.21 or later
* gcc v15.2.0 or later
  * i686-w64-mingw32-g++
  * i686-w64-mingw32-gcc
* An internet connection

For Windows users:
* MSYS2 environment (http://msys2.org)
* In a MINGW32 shell, make sure you have access to gcc and cmake

### Instructions
Run the Compile.sh script in the project root directory (from a MINGW32 shell for Windows users).
This make take a minute or two the first time as CMake fetches dependencies.

Alternatively, you can manually configure and build via CMake.
When cross-compiling from Linux/iOS use the `toolchain.cmake` file
when configuring the build files

### Troubleshooting
If you are getting assembler errors, a vague reference to `nativeFunctions.cpp`, and are using a compiler other than gcc, the issue is likely from incompatibilies in the asm declarations in `nativeFunctions.cpp`. Either use gcc or you can try adapting the short asm declaration to your compiler of choice.
