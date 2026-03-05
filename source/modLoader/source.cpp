#include "gearLoaderApi/gearLoader_p.h"
#include <algorithm>
#include <cstring>
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


typedef BOOL (__stdcall *NativeInitFunctionPrototype)();
static NativeInitFunctionPrototype _nativeInit;
BOOL __stdcall NativeInitWrapper() {
    BOOL result = 0;
    if (_nativeInit) result = _nativeInit();
    if (result) Main(NULL);
    return result;
}
inline bool CheckMemory(uint8_t* callAddress, uint8_t* initFunctionAddress) {
    // callAddress is the instruction at GGXXACPR_Win.exe+2223A3 that calls
    //  an initialization helper function in the main function.
    //  The expected values are the machine code for this call instruction
    static const uint8_t callAddressExpected[] = { 0xe8, 0x78, 0xf3, 0xff, 0xff };
    // initFunctionAddress is the address of this helper function at GGXXACPRR_Win.exe+221720.
    //  The expected values are the first couple assembly operations of the function.
    static const uint8_t initFunctionAddressExpected[] = { 0x53, 0x56, 0x57, 0x68 };

    return std::equal(callAddress, callAddress+5, callAddressExpected) &&
        std::equal(initFunctionAddress, initFunctionAddress+4, initFunctionAddressExpected);
}
DWORD WINAPI PatchInModLoader(LPVOID lpParameter) {
    if (!OriginalFunc) {
        loadOriginalDllFunction();
    }
    if (!isTargetProcess()) {
        _logger.log(VERBOSE, "The Steam appId was incorrect. Initialization halted.");
        return 0;
    }
    if (args.debugConsole) {
        InitConsole();
        std::cout << "[GearLoader] Debug Console initialized" << std::endl;
    }

    intptr_t baseAddress = reinterpret_cast<intptr_t>(GetModuleHandle(nullptr));
    intptr_t callAddress = baseAddress + 0x2223A3;
    void* injectAddress = reinterpret_cast<void*>(callAddress + 1);   // +1 to skip opcode
    _nativeInit = reinterpret_cast<NativeInitFunctionPrototype>(baseAddress + 0x221720);
    intptr_t hookAddress = reinterpret_cast<intptr_t>(NativeInitWrapper);

    if (!CheckMemory(reinterpret_cast<uint8_t*>(callAddress), reinterpret_cast<uint8_t*>(_nativeInit))) {
        _logger.log(ERR, "Unexpected memory detected. The game is an incompatible version or incompatible mods are installed.");
        return 1;
    }

    intptr_t callOffset = hookAddress - reinterpret_cast<intptr_t>(injectAddress) - sizeof(DWORD);


    DWORD oldProtect;
    WINBOOL success = VirtualProtect(
        injectAddress,
        sizeof(callOffset),
        PAGE_EXECUTE_READWRITE,
        &oldProtect
    );
    if (!success) {
        _logger.log(ERR, "VirtualProtect failed: 0x%x", GetLastError());
        return 1;
    }

    DWORD overwritten;
    std::memcpy(&overwritten, injectAddress, sizeof(DWORD));
    std::memcpy(injectAddress, &callOffset, sizeof(callOffset));

    success = VirtualProtect(
        injectAddress,
        sizeof(callOffset),
        oldProtect,
        &oldProtect
    );
    if (!success) {
        _logger.log(ERR, "VirtualProtect failed: 0x%x", GetLastError());
        return 1;
    }

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        HANDLE initThread = CreateThread(NULL, 0, PatchInModLoader, NULL, 0, NULL);
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
