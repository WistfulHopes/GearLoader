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

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

enum LogLevel : int { DEBUG, INFO, WARN, ERR, VERBOSE };

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

class SpdLogger {
public:
    SpdLogger(std::string fileName, bool verbose = false) {
        _underlying = spdlog::basic_logger_mt(fileName, fileName);
        _underlying->set_pattern("[%Y-%m-%d %T:%e] [%l]\t%v");
    }
    template<typename... Args>
    void log(std::string format, Args&&... args) {
        log(LogLevel::DEBUG, format, std::forward<Args>(args)...);
    }
    template<typename... Args>
    void log(LogLevel level, std::string format, Args&&... args) {
        switch(level) {
            case DEBUG:
                _underlying->debug(compose(format, std::forward<Args>(args)...));
                break;
            case INFO:
                _underlying->info(compose(format, std::forward<Args>(args)...));
                break;
            case WARN:
                _underlying->warn(compose(format, std::forward<Args>(args)...));
                break;
            case ERR:
                _underlying->error(compose(format, std::forward<Args>(args)...));
                break;
            case VERBOSE:
                _underlying->info(compose(format, std::forward<Args>(args)...));
                break;
        }
    }
private:
    std::shared_ptr<spdlog::logger> _underlying;
    template<typename... Args>
    inline std::string compose(std::string& format, Args&&... args) {
        std::stringstream ss;
        char buffer[1024];
        
        snprintf(buffer, sizeof(buffer), format.c_str(), std::forward<Args>(args)...);
        ss << buffer;
        return ss.str();
    }
};

class CustomLogger {
public:
    CustomLogger(std::string fileName, bool verbose = false) :
        _fileName(fileName),
        _verbose(verbose) { }
    ~CustomLogger() {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            stop = true;
        }
        _workerThreadCV.notify_all();
        if (_workerThread.joinable()) _workerThread.join();
    }
    template<typename... Args>
    void log(const char* format, Args&&... args) {
        log(LogLevel::DEBUG, format, std::forward<Args>(args)...);
    }
    template<typename... Args>
    void log(LogLevel level, const char* format, Args&&... args) {
        if (stop || (!_verbose && level == VERBOSE)) return;
        
        lazyLoadWorkerThread();
        
        std::stringstream ss;
        std::tm tm;
        time_t timeStamp = time(nullptr);
        localtime_s(&tm, &timeStamp);
        char buffer[1024];
        std::strftime(buffer, sizeof(buffer), "%c", &tm);
        ss << buffer << "\t";
        ss << std::format("{:10}", "[" + toString(level) + "]");
        
        snprintf(buffer, sizeof(buffer), format, std::forward<Args>(args)...);
        ss << buffer << std::endl;

        {
            std::lock_guard<std::mutex> lock(_mutex);
            _queue.push(ss.str());
        }
        _workerThreadCV.notify_one();
    }

private:
    std::string _fileName;
    bool _verbose;

    std::mutex _mutex;
    std::thread _workerThread;
    std::once_flag _threadInitialized;
    std::atomic<bool> stop = false;
    std::condition_variable _workerThreadCV;

    std::queue<std::string> _queue;
    std::ofstream _logFile;

    void writeLogsToFile() {
        _logFile.open(_fileName, std::ios::app);
        if (!_logFile.is_open()) {
            std::cerr << "[GEARLOADER] couldn't open log file: " << _fileName << std::endl;
            return;
        }
        
        while (true) {
            std::unique_lock<std::mutex> lock(_mutex);
            _workerThreadCV.wait(lock, [this] {
                return stop || !_queue.empty();
            });

            if (stop && _queue.empty())
                break;

            std::string msg;
            for (/*blank*/; !_queue.empty(); _queue.pop()) {
                msg += std::move(_queue.front());
            }
            lock.unlock();

            _logFile << msg;
        }
    }

    void lazyLoadWorkerThread() {
        std::call_once(_threadInitialized, [this] {
            _workerThread = std::thread(&CustomLogger::writeLogsToFile, this);
        });
    }
};

using Logger = SpdLogger;
