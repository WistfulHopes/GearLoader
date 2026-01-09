#include "gearLoaderApi/gearLoader_p.h"
#include <string>
#include <filesystem>
#include <windows.h>
#include "Patch/patch.h"
#include "logger/logger.h"
#include "dependencyManager/dependencyManager.h"
#include "modFolderWalker/modFolderWalker.h"

namespace fs = std::filesystem;

typedef BOOL(WINAPI *MiniDumpWriteDump_t)(HANDLE, DWORD, HANDLE, int, const void *, const void *, const void *);

static LPCSTR _targetAppId = "348550";


static Logger _logger("GearLoader.log", true);
Logger& GetLogger() {
    return _logger;
}

static DependencyManager _depMan;
static GearLoaderApi* _gearLoaderApi = GetGearLoaderAPI();

// TEMP (eventually moving this functionality to base mod)
typedef void(*VoidFunc)(void);
VoidFunc HitStopTimeExeRoot;
int GLFunctionsCnt = 0;
VoidFunc GameLoopFunctions[256] = {0};
// END TEMP

HMODULE dbghelp;
MiniDumpWriteDump_t OriginalFunc = NULL;

// TEMP (eventually moving this functionality to base mod)
void GameLoopExecute(void) {
    HitStopTimeExeRoot();
    for (int i = 0; i < GLFunctionsCnt; i++) {
        VoidFunc func = GameLoopFunctions[i];
        if (func != NULL) {
            func();
        }
    }
    return;
}
void HookGameLoop(void) {
    BYTE *base = GetBase();
    HitStopTimeExeRoot = (VoidFunc)(base + 0x2af6a0);
    char call[5] = {(char)0xE8};
    *(DWORD*)(call+1) = (DWORD)GameLoopExecute - ((DWORD)base+0x1c2fe7 + 5);
    PatchSafe(base+0x1c2fe7,call,5);
    return;
}
// END TEMP


void LoadAndInitMod(ModManifest& manifest) {
    static int loadOrder = 0;
    GearLoaderContext ctx = {
        manifest,
        loadOrder++,
        _logger
    };

    _logger.log("Attempting to load manifest for \"%s\" at \"%s\"", manifest.name.c_str(), manifest.path.string().c_str());
    std::wstring libraryPath = manifest.path.wstring();
    HMODULE modHandle = LoadLibraryW(libraryPath.c_str());

    if (modHandle == NULL) {
        int errCode = GetLastError();
        _logger.log(ERR, "Failed to load library \"%s\". Error code: %d", manifest.path.string().c_str(), errCode);
    }

    ModInitFunc fInit = (ModInitFunc)GetProcAddress(modHandle, "Init");

    if (!fInit) {
        DWORD err = GetLastError();
        _logger.log(WARN, "No Init function (err 0x%x) found for mod \"%s\"", err, manifest.path.filename().string().c_str());
        return;
    }
    
    _logger.log(VERBOSE, "Initializing Module Handle: 0x%x", modHandle);
    fInit(&ctx, _gearLoaderApi);
}

void AddToDependencyMananager(fs::directory_entry modFolder, fs::directory_entry file) {
    if (file.path().filename().compare("config.json") == 0) {
        _logger.log(VERBOSE, "config.json found in folder: %s", modFolder.path().string().c_str());
        ModManifest parsedManifest = ParseConfig(file.path());
        _logger.log(VERBOSE, "mod manifest parsed.");
        _depMan.registerManifest(parsedManifest);
    }
}

inline bool ConsoleFlag() {
    std::string cmdLine = GetCommandLine();

    return cmdLine.find("DebugConsole") != std::string::npos;
}
inline void InitConsole() {
    AllocConsole();

    FILE* fpOut;
    freopen_s(&fpOut, "CONOUT$", "w", stdout);
    freopen_s(&fpOut, "CONOUT$", "w", stderr);

    FILE* fpIn;
    freopen_s(&fpIn, "CONIN$", "r", stdin);

    std::ios::sync_with_stdio(true);
}


inline void loadOriginalDllFunction() {
    char DBGOriginal[MAX_PATH];
    GetSystemDirectoryA(DBGOriginal,MAX_PATH);
    strcat(DBGOriginal,"\\dbghelp.dll");
    dbghelp = LoadLibraryA(DBGOriginal);
    if (dbghelp) {
        OriginalFunc = (MiniDumpWriteDump_t)GetProcAddress(dbghelp,"MiniDumpWriteDump");
    }
}
// SteamAppId is checked mainly because of the relaunch behavior when launching +R directly from
//  its exe. The game will immediately relaunch under a Steam environment when launched that way.
//  The main issue is this leads to meaningless errors in the log file.
inline bool isTargetProcess() {
    char appId[128];
    GetEnvironmentVariableA("SteamAppId", appId, 128);

    return strcmp(appId, _targetAppId) == 0;
}
DWORD WINAPI Main(LPVOID lpParameter) {
    _logger.log(INFO, "Gear Loader Initializing...");
    if (!OriginalFunc) {
        loadOriginalDllFunction();
    }

    if (!isTargetProcess()) {
        _logger.log(WARN, "The Steam appId was incorrect");
        return 0;
    }

    if (ConsoleFlag()) {
        InitConsole();
        std::cout << "[GearLoader] Debug Console initialized" << std::endl;
    }
    
    // TEMP (eventually moving this functionality to base mod)
    HookGameLoop();

    WalkModFolder(fs::current_path() / "mods", AddToDependencyMananager, _logger);

    _depMan.finalize(_logger);
    _logger.log(VERBOSE, _depMan.printGraph().c_str());

    for (ModManifest& iMani : _depMan.createLoadOrderVector()) {
        LoadAndInitMod(iMani);
    }

    return 0;
}


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        HANDLE initThread = CreateThread(NULL, 0, Main, NULL, 0, NULL);
        CloseHandle(initThread);
    }
    return TRUE;
}

extern "C" void AssignToGameLoop(VoidFunc func) {
    GameLoopFunctions[GLFunctionsCnt] = func;
    GLFunctionsCnt += 1;
    return;
}

extern "C" BOOL WINAPI MiniDumpWriteDump(
    HANDLE hProcess, DWORD ProcessId, HANDLE hFile, int DumpType, 
    const void *Exception, const void *UserStream, const void *Callback)
{
    if (OriginalFunc) {
        return OriginalFunc(hProcess,ProcessId,hFile,DumpType,Exception,UserStream,Callback);
    }
    return FALSE;
}
