#include "gearLoaderApi/gearLoader_p.h"
#include <algorithm>
#include <string>
#include <filesystem>
#include <windows.h>
#include "common/versionParsing.h"
#include "logger/logger.h"
#include "dependencyManager/dependencyManager.h"
#include "modFolderWalker/modFolderWalker.h"

namespace fs = std::filesystem;

typedef BOOL(WINAPI *MiniDumpWriteDump_t)(HANDLE, DWORD, HANDLE, int, const void *, const void *, const void *);
static MiniDumpWriteDump_t OriginalFunc = NULL;

static DependencyManager _depMan;
static GearLoaderApi* _gearLoaderApi = GetGearLoaderAPI();

static LPCSTR _targetAppId = "348550";


struct CmdLineArgs {
    bool verbose = false;
    bool debugConsole = false;
};
static const char* verboseFlag = "-gearloaderverbose";
static const char* consoleFlag = "-debugconsole";
inline CmdLineArgs ParseCommandLineArgs() {
    std::string cmdLine = GetCommandLine();
    CmdLineArgs output;

    std::transform(cmdLine.begin(), cmdLine.end(), cmdLine.begin(),
        [](unsigned char c){ return std::towlower(c); });

    std::stringstream stream(cmdLine);
    std::string token;

    while (std::getline(stream, token, ' ')) {
        if (token.compare(verboseFlag) == 0) {
            output.verbose = true;
        } else if (token.compare(consoleFlag) == 0) {
            output.debugConsole = true;
        }
    }

    return output;
}
static CmdLineArgs args = ParseCommandLineArgs();

static Logger _logger("GearLoader.log", args.verbose);


void LoadAndInitMod(ModManifest& manifest) {
    static GearLoaderContext _contextBuffer[MAX_LOADED_MODS];
    static int _loadOrder = 0;

    _contextBuffer[_loadOrder] = GearLoaderContext {
        GEARLOADER_VERSION_SEM_VER,
        &manifest,
        _loadOrder,
        &_logger
    };
    GearLoaderContext* currentContext = &_contextBuffer[_loadOrder];

    std::wstring libraryPath = manifest.path.wstring();
    HMODULE modHandle = LoadLibraryW(libraryPath.c_str());

    if (modHandle == NULL) {
        DWORD errCode = GetLastError();
        _logger.log(ERR,
            "Failed to load library \"%s\". Error code: %d",
            manifest.path.string().c_str(),
            errCode);
        return;
    }

    ModInitFunc fInit = reinterpret_cast<ModInitFunc>(GetProcAddress(modHandle, "Init"));

    if (fInit == NULL) {
        DWORD errCode = GetLastError();
        _logger.log(WARN,
            "No Init function found for mod \"%s\". Error code: %d",
            manifest.path.filename().string().c_str(),
            errCode);
        return;
    }
    
    _logger.log(VERBOSE, "Invoking Init function for Mod: \"%s\" Handle: 0x%x", manifest.name.c_str(), modHandle);
    fInit(currentContext, _gearLoaderApi);
    _logger.log(INFO, "Mod \"%s\" v%s initialized", manifest.name.c_str(), ToString(manifest.version).c_str());
}

void AddToDependencyMananager(fs::directory_entry modFolder, fs::directory_entry file) {
    if (file.path().filename().compare("config.json") == 0) {
        _logger.log(VERBOSE, "config.json found in folder: %s", modFolder.path().string().c_str());
        
        ModManifest parsedManifest = ParseConfig(file.path());
        _logger.log(VERBOSE, "config successfully parsed.");

        if (!parsedManifest.ignore)
            _depMan.registerManifest(parsedManifest);
        else
            _logger.log(VERBOSE, "Ignoring mod \"%s\"", parsedManifest.name.c_str());
    }
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
    static HMODULE dbghelp;

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
    _logger.log(INFO, "Gear Loader v%s Initializing...", GEARLOADER_VERSION);
    if (!OriginalFunc) {
        loadOriginalDllFunction();
    }

    if (!isTargetProcess()) {
        _logger.log(WARN, "The Steam appId was incorrect");
        return 0;
    }

    if (args.debugConsole) {
        InitConsole();
        std::cout << "[GearLoader] Debug Console initialized" << std::endl;
    }

    WalkModFolder(fs::current_path() / "mods", AddToDependencyMananager, _logger);

    _depMan.finalize(_logger);
    _logger.log(VERBOSE, "Final load order:\n%s", _depMan.printGraph().c_str());

    // TODO: this is messy
    // Static instantiation to keep mod manifest references alive for `LoadAndInitMod::_contextBuffer`
    //  Alternatively, `GearLoaderContext` struct can be changed to store a direct copy of the manifest
    static std::vector<ModManifest> loadOrder = _depMan.createLoadOrderVector();

    for (ModManifest& iMani : loadOrder) {
        LoadAndInitMod(iMani);
    }

    _logger.log(INFO, "Mod initialization complete");
    return 0;
}


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        HANDLE initThread = CreateThread(NULL, 0, Main, NULL, 0, NULL);
        CloseHandle(initThread);
    }
    return TRUE;
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
