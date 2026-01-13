#include "apiRegistry/apiRegistry.h"
#include "dependencyManager/configParser.h"
#include "dependencyManager/dependencyManager.h"
#include "gearLoaderApi/gearLoader_p.h"
#include "logger/logger.h"
#include "modFolderWalker/modFolderWalker.h"
#include <exception>
#include <string>
#include <windows.h>

typedef void(*TestFunc)();

static std::string mainTestLogFileName = "./test/test.log";
Logger initTestLogger() {
    if (std::filesystem::exists(mainTestLogFileName)) {
        std::filesystem::remove(mainTestLogFileName);
    }
    return Logger(mainTestLogFileName, true);
}
static Logger _testLogger = initTestLogger();

inline void assert(bool val, std::string assertionName = "unnamed") {
    if (!val) throw std::runtime_error(assertionName + " assertion failed");
}

void testLogger() {
    std::string testLogFileName = "./test/testLogs/test_nonverbose.log";
    std::string testLogVerboseFileName = "./test/testLogs/test_verbose.log";

    if (std::filesystem::exists(testLogFileName)) {
        std::filesystem::remove(testLogFileName);
    }
    if (std::filesystem::exists(testLogVerboseFileName)) {
        std::filesystem::remove(testLogVerboseFileName);
    }

    Logger _testLog1(testLogFileName, false);
    _testLog1.log(VERBOSE, "Testing the debug Logger VERBOSE");
    _testLog1.log(DEBUG, "Testing the debug Logger DEBUG");
    _testLog1.log(INFO, "Testing the debug Logger INFO with args %d", 20);
    _testLog1.log(WARN, "Testing the debug Logger WARN");
    _testLog1.log(ERR, "Testing the debug Logger ERROR");

    Logger _testLog2(testLogVerboseFileName, true);
    _testLog2.log(VERBOSE, "Testing the debug Logger VERBOSE");
    _testLog2.log(DEBUG, "Testing the debug Logger DEBUG");
    _testLog2.log(INFO, "Testing the debug Logger INFO with args %d", 20);
    _testLog2.log(WARN, "Testing the debug Logger WARN");
    _testLog2.log(ERR, "Testing the debug Logger ERROR");

    assert(std::filesystem::exists(testLogFileName),
        "log file exists");
    assert(std::filesystem::exists(testLogVerboseFileName),
        "verbose log file exists");
}

void testAPIRegistry() {
    APIRegistry _modReg;
    SemanticVersion semVer1 = {1,0,0};

    _modReg.put((void*)0x10, "Test1", semVer1, &_testLogger);
    _modReg.put((void*)0x20, "Test2", SemanticVersion{1,1,0}, &_testLogger);
    bool duplicatePutReturn = _modReg.put((void*)0x10, "Test1", semVer1, &_testLogger);
    // Multiple API versions not yet implemented
    // _modReg.put((void*)0x30, "Test2", SemanticVersion{1,2,0}, &_testLogger);

    ModApi retApi1;
    ModApi retApi2;
    ModApi fooApi;
    bool getSuccess1 = _modReg.get("Test1", retApi1, &_testLogger);
    bool getSuccess2 = _modReg.get("Test2", retApi2, &_testLogger);
    bool garbageGet = _modReg.get("Foobar", fooApi, &_testLogger);

    assert(duplicatePutReturn == false,
        "Duplicate put returns false");
    assert(getSuccess1 == true,
        "Get 1 succeded");
    assert(getSuccess2 == true,
        "Get 2 succeded");
    assert(garbageGet == false,
        "Requesting unregistered API should return false");
    assert(retApi1.api == (void*)0x10,
        "api retrieval 1");
    assert(retApi1.version == semVer1,
        "api version");
    assert(retApi2.api == (void*)0x20,
        "api retrieval 2");
    assert(retApi2.version == SemanticVersion{1,1,0},
        "api version 2");
}

void testConfigParser() {

    std::filesystem::path path = std::filesystem::current_path() / "test/testConfig.json";
    ModManifest manifest = ParseConfig(path);

    assert(manifest.name.compare("test-mod") == 0,
        "parse mod name");
    assert(manifest.version == SemanticVersion{1,0,0},
        "parse version");
    assert(manifest.dependencies.size() == 3,
        "parse dependency size");
    assert(manifest.dependencies[0].name.compare("test-dependency") == 0,
        "parse first dependency name");
    assert(manifest.dependencies[1].name.compare("test-dependency-2") == 0,
        "parse second dependency name");
    assert(manifest.dependencies[1].minVersion == SemanticVersion{0,2,6},
        "parse second dependency version");

    std::filesystem::path modAPath = std::filesystem::current_path() / "test/mods/modA/config.json";
    ModManifest manifest2 = ParseConfig(modAPath);
    assert(manifest2.name.compare("modA") == 0,
        "parse modA name");
    assert(manifest2.path.filename().compare("dummy.dll") == 0,
        "correct dll path");
}

void testSemVarParsing() {
    std::string str1 = "1.0.0";
    std::string str2 = "1.2.3";
    std::string str3 = "3";
    std::string str4 = "foobar";

    SemanticVersion version = toSemanticVersion(str1);
    assert(version == SemanticVersion{1,0,0},
        "parse 1.0.0");

    version = toSemanticVersion(str2);
    assert(version == SemanticVersion{1,2,3},
        "parse 1.2.3");

    version = toSemanticVersion(str3);
    assert(version == SemanticVersion{3,0,0},
        "parse 3.0.0");

    version = toSemanticVersion(str4);
    assert(version == SemanticVersion{0,0,0},
        "parse foobar");
}

inline bool searchFile(std::string filePath, std::string str) {
    std::ifstream stream;
    stream.open(filePath);

    std::string line;
    while(std::getline(stream, line, '\n')) {
        if (line.find(str) != std::string::npos) {
            return true;
        }
    }

    return false;
}

void testDependencyManager() {
    std::string depManLogFile = "./test/testLogs/DependencyManagerTest1.log";
    if (std::filesystem::exists(depManLogFile)) {
        std::filesystem::remove(depManLogFile);
    }

    Logger log(depManLogFile, true);

    // prep test data
    std::vector<DependencyManifest> depList = {
        DependencyManifest{"modB", SemanticVersion{1,1,0}},
        DependencyManifest{"modC", SemanticVersion{1,0,0}}
    };
    ModManifest modA = ModManifest{
        "modA",
        SemanticVersion{1,0,0},
        "./fakeA",
        depList
    };
    ModManifest modB = ModManifest{
        "modB",
        SemanticVersion{1,1,0},
        "./fakeB"
    };
    ModManifest modC = ModManifest{
        "modC",
        SemanticVersion{1,0,0},
        "./fakeC"
    };

    DependencyManager depMan;

    depMan.registerManifest(modA);
    depMan.registerManifest(modB);
    depMan.registerManifest(modC);
    depMan.finalize(log);

    std::string graphStr = depMan.printGraph();

    int lineCount = 0;
    for(const char c : graphStr) {
        if (c == '\n') lineCount++;
    }

    log.log(DEBUG, ("mod load order:\n" + graphStr).c_str());

    assert(lineCount == 2,
        "graph string line count check");
    assert(!searchFile(depManLogFile, "ERROR"),
        "No errors from logger");
}

void testDependencyManagerCycles() {
    std::string depManLogFile = "./test/testLogs/DependencyManagerTest2.log";
    if (std::filesystem::exists(depManLogFile)) {
        std::filesystem::remove(depManLogFile);
    }

    Logger log(depManLogFile, true);

    // prep test data
    std::vector<DependencyManifest> depListA = {
        DependencyManifest{"modB", SemanticVersion{1,0,0}},
    };
    std::vector<DependencyManifest> depListB = {
        DependencyManifest{"modC", SemanticVersion{1,0,0}},
    };
    std::vector<DependencyManifest> depListC = {
        DependencyManifest{"modA", SemanticVersion{1,0,0}},
    };
    ModManifest modA = ModManifest{
        "modA",
        SemanticVersion{1,0,0},
        "./fakeA",
        depListA
    };
    ModManifest modB = ModManifest{
        "modB",
        SemanticVersion{1,0,0},
        "./fakeB",
        depListB
    };
    ModManifest modC = ModManifest{
        "modC",
        SemanticVersion{1,0,0},
        "./fakeC",
        depListC
    };
    ModManifest modD = ModManifest{
        "modD",
        SemanticVersion{1,2,3},
        "./fakeD"
    };

    DependencyManager depMan;

    depMan.registerManifest(modA);
    depMan.registerManifest(modB);
    depMan.registerManifest(modC);
    depMan.registerManifest(modD);
    depMan.finalize(log);

    std::string graphStr = depMan.printGraph();
    log.log(DEBUG, ("mod load order:\n" + graphStr).c_str());

    assert(searchFile(depManLogFile, "ERROR"),
        "No errors from logger");
}

void testModFolderWalker() {
    std::filesystem::path testModFolder = std::filesystem::current_path() / "test\\mods";
    std::filesystem::path curDirectory = std::filesystem::current_path();

    WalkModFolder(
        testModFolder,
        [](std::filesystem::directory_entry modFolder, std::filesystem::directory_entry file) {
            // intentionally blank
        },
        _testLogger);
}

inline void test(std::string testName, TestFunc testFunc) {
    std::cout << testName;

    try {
        testFunc();
        std::cout << "pass";
    }
    catch (std::runtime_error e) {
        std::cout << "fail\t" << e.what();
    }
    
    std::cout << std::endl;
}

int main() {
    test("Logger tests             ", testLogger);
    test("API Registry tests       ", testAPIRegistry);
    test("Config parsing tests     ", testConfigParser);
    test("Semantic Version parsing ", testSemVarParsing);
    test("Dependency manager tests ", testDependencyManager);
    test(" - Circular Dep test     ", testDependencyManagerCycles);
    test("Mod Folder Walker test   ", testModFolderWalker);

    std::cout << std::endl << "See " << mainTestLogFileName << " for more information" << std::endl;

    return 0;
}