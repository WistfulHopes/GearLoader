# Install instructions

Move "DBGHELP.dll" and the "mods" folder into the "Guilty Gear XX Accent Core Plus R" folder.

For Linux users:
You must also add the following launch option for +R:

    WINEDLLOVERRIDES="dbghelp=n,b" %command%


Check for the "GearLoader.log" file in the game folder after launching the game to confirm installation.


# Additional launch options

-GearLoaderVerbose
Includes verbose logging to the GearLoader.log file

-DebugConsole
Launches the game with a debug console. Useful for mod developers.
