#pragma once
#include <chrono>
#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <ostream>

enum LogLevel : int { DEBUG, INFO, WARN, ERR, VERBOSE };

class Logger {
public:
    Logger(std::string fileName, bool verbose = false) {
        _verbose = verbose;
        logFile.open(fileName, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "[GEARLOADER] couldn't open log file: " << fileName << std::endl;
        }
    }
    ~Logger() {
        if (logFile.is_open()) logFile.close();
    }
    template<typename... Args>
    void log(const char* format, Args&&... args) {
        log(LogLevel::DEBUG, format, std::forward<Args>(args)...);
    }
    template<typename... Args>
    void log(LogLevel level, const char* format, Args&&... args) {
        if (!logFile.is_open()) return;
        if (!_verbose && level == VERBOSE) return;

        time_t timeStamp = time(NULL);
        std::strftime(_buffer, sizeof(_buffer), "%c", std::localtime(&timeStamp));
        logFile << _buffer << "\t";

        logFile << std::format("{:10}", "[" + toString(level) + "]");

        snprintf(_buffer, sizeof(_buffer), format, std::forward<Args>(args)...);
        logFile << _buffer << std::endl;
    }
    std::ofstream* getFileStream() {
        return &logFile;
    }

private:
    bool _verbose;
    char _buffer[1024];
    std::ofstream logFile;
    inline std::string toString(LogLevel level) {
        switch(level) {
            case DEBUG: return "DEBUG";
            case INFO: return "INFO";
            case WARN: return "WARN";
            case ERR: return "ERROR";
            case VERBOSE: return "VERBOSE";
            default: return "UNDEFINED";
        }
    }
};