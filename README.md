# - DLL Loader for GGXXACPR (Steam) -
Original Gear-DLL-Loader by: lovenus
GearLoader by: YouKnow

Version: 0.1.0

A DLL-based mod loader and manager for Guilty Gear Accent Core Plus R.

## Instructions
The DBGHELP.DLL must be placed in the same folder as GGXXACPR_Win.exe 
and any mod that requires it should be placed under a Mods folder 
that you must create yourself (that also has to be in the same folder 
as GGXXACPR_Win.exe, NOT inside Resource).

When playing on Linux/Steam Deck, add `WINEDLLOVERRIDES="dbghelp=n,b" %command%` to the game's launch options.

### Modding info
The mod loader can interface with C/C++ DLLs. Each mod must be contained in its own subfolder in the mods folder. Each mod must have a config.json file (see example mods) and a single DLL in its folder. Additional DLL dependencies may be included in a subdirectory.

Each mod Dll may export an `Init` function (see example mods), but it is not required to be loaded.

## Build Instructions
### Prerequisites

* CMake v3.21 or later
* gcc v15.2.0 or later
  * i686-w64-mingw32-g++
  * i686-w64-mingw32-gcc

For Windows users:
* MSYS2 environment (http://msys2.org)
* In a MINGW32 shell, make sure you have access to gcc and cmake


### Instructions
Launch the Compile.sh script in the project root directory (from a MINGW32 shell for Windows users).

Alternatively, you can manually configure and build via CMake.
When cross-compiling from Linux/iOS use the `toolchain.cmake` file
when configuring the build files
