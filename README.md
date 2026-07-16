# GearLoader - Mod Loader for GGXXACPR (Steam)
Based on the original [Gear-DLL-Loader](https://github.com/MoonSquaredd/Gear-DLL-Loader) by [lovenus](https://github.com/MoonSquaredd)


GearLoader: v0.1.0 <br>
baseMod: v1.1.1

GearLoader is a DLL-based mod loader and manager for the Steam release of Guilty Gear Accent Core Plus R. GearLoader comes bundled with baseMod, a modding API to support mod development.


## Install Instructions
Grab the latest "GearLoader.zip" from [releases](https://github.com/YouKnow232/GearLoader/releases), unpack it and place
its contents in your game folder where GGXXACPR_Win.exe is.
Any mod that requires it should be placed in the mods folder.

```text
Guilty Gear XX Accent Core Plus R/
├── GGXXACPR_Win.exe
├── DBGHELP.dll
└── mods/
    └── baseMod/
```

When installing on Linux/Steam Deck, add `WINEDLLOVERRIDES="dbghelp=n,b" %command%` to the game's launch options.

Additional launch options include:
* `-GearLoaderVerbose` for verbose logging to `GearLoader.log`
* `-DebugConsole` to open a console window on launch


## Making a Mod
See the tutorial and documentation [here!](https://youknow232.github.io/GearLoader/doxygen/md_docs_2_modding_instructions.html)


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
Run the `Compile.sh` script in the project root directory (from a MINGW32 shell for Windows users).
This may take a minute or two the first time as CMake fetches dependencies.

Alternatively, you can manually configure and build via CMake.
If cross-compiling from Linux/iOS use the `toolchain.cmake` file
when configuring the build files.


### Troubleshooting
If you are getting assembler errors, a vague reference to `nativeFunctions.cpp`, and are using a compiler other than gcc, the issue is likely from incompatibilities in the asm declarations in `nativeFunctions.cpp`. Either use gcc or you can try adapting the short asm declarations to your compiler of choice.
