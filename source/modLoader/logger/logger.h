#pragma once
#include <chrono>
#include <cstdio>
#include <condition_variable>
#include <mutex>
#include <string>
#include <queue>
#include <sstream>
#include <fstream>
#include <iostream>
#include <ostream>
#include <thread>

enum LogLevel : int { DEBUG, INFO, WARN, ERR, VERBOSE };

class Logger {
public:
    Logger(std::string fileName, bool verbose = false) :
        _fileName(std::move(fileName)),
        _verbose(verbose),
        _workerThread(&Logger::writeLogsToFile, this) {
            std::unique_lock<std::mutex> lock(_mutex);
            _workerThreadReady.wait(lock, [this] { return workerThreadReady; });
        }
    ~Logger() {
        stop = true;
        _workQueued.notify_one();
        if (_workerThread.joinable()) _workerThread.join();
    }
    template<typename... Args>
    void log(const char* format, Args&&... args) {
        log(LogLevel::DEBUG, format, std::forward<Args>(args)...);
    }
    template<typename... Args>
    void log(LogLevel level, const char* format, Args&&... args) {
        std::stringstream ss;
        if (!_logFile.is_open() || !_verbose && level == VERBOSE) return;
        
        time_t timeStamp = time(nullptr);
        std::strftime(_buffer, sizeof(_buffer), "%c", std::localtime(&timeStamp));
        ss << _buffer << "\t";
        ss << std::format("{:10}", "[" + toString(level) + "]");
        
        snprintf(_buffer, sizeof(_buffer), format, std::forward<Args>(args)...);
        ss << _buffer << std::endl;

        {
            std::lock_guard<std::mutex> lock(_mutex);
            _queue.push(ss.str());
        }
        _workQueued.notify_one();
    }

private:
    std::queue<std::string> _queue;
    std::mutex _mutex;
    std::condition_variable _workQueued;
    std::condition_variable _workerThreadReady;
    bool _verbose;
    std::string _fileName;
    char _buffer[1024];
    bool workerThreadReady = false;
    bool stop = false;
    std::ofstream _logFile;
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

    std::thread _workerThread;
    void writeLogsToFile() {
        _logFile.open(_fileName, std::ios::app);
        if (!_logFile.is_open()) {
            std::cerr << "[GEARLOADER] couldn't open log file: " << _fileName << std::endl;
            return;
        }
        workerThreadReady = true;
        _workerThreadReady.notify_one();
        
        while (!stop) {
            std::unique_lock<std::mutex> lock(_mutex);
            _workQueued.wait(lock, [this] { return !_queue.empty() || stop; });
            
            while (!_queue.empty()) {
                try {
                    _logFile << _queue.front();
                } catch (const std::exception& e) {
                    std::cerr << "[GEARLOADER] Error writing log to file: " << e.what() << std::endl;
                }
                _queue.pop();
            }
        }
        _logFile.flush();
    }
};