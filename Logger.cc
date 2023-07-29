#include "Logger.h"
#include "Timestamp.h"
#include <mutex>
#include <iostream>

Logger &Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(int level) {
    logLevel_ = level;
}

void Logger::log(const std::string &Msg) const {
    static std::mutex mtx;

    std::lock_guard<std::mutex> lock(mtx);
    switch (logLevel_) {
        case INFO:
            std::cout << "[INFO]";
            break;
        case ERROR:
            std::cout << "[ERROR]";
            break;
        case FATAL:
            std::cout << "[FATAL]";
            break;
        case DEBUG:
            std::cout << "[DEBUG]";
            break;
        default:
            break;
    }

    std::cout << Timestamp::now().toString() << " : " << Msg << std::endl;
}