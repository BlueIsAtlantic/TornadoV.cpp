#pragma once
#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    static void Initialize(const std::string& filePath);
    static void Log(const std::string& message);
    static void Error(const std::string& message);

private:
    static std::string m_filePath;
    static std::mutex m_mutex;
};
