# - DLL Loader for GGXXACPR (Steam) -
by: lovenus

Version: 2.0.0

A Loader used to load custom DLLs into the game.

## Instructions
The DBGHELP.DLL must be placed in the same folder as GGXXACPR_Win.exe 
and any mod that requires it should be placed under a Mods folder 
that you must create yourself (that also has to be in the same folder 
as GGXXACPR_Win.exe, NOT inside Resource).

### Modding info
The mod loader can interface with C/C++ DLLs. Each mod must be contained in its own subfolder in the mods folder. Each mod must have a config.json file (see example mods) and a single DLL in its folder. Additional DLL dependencies may be included in a subdirectory.

Each mod Dll may export an `Init` function (see example mods), but it is not required to be loaded.

## Build Instructions
### Prerequisites
For Windows users:
* MSYS2 environment
* In a MINGW32 shell, make sure you have access to:
* * gcc v15.2.0 or later
* * CMake v3.21 or later

### Instructions
Launch the Compile.sh script in the project root directory (from a MINGW32 shell for Windows users).

Alternatively, you can manually configure and build via CMake.
