#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <filesystem>

std::string Logger::m_filePath = "";
std::mutex Logger::m_mutex;

void Logger::Initialize(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_filePath = filePath;
    
    // Create directory if it doesn't exist
    std::filesystem::path p(m_filePath);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }
    
    // Clear the log file on startup
    std::ofstream file(m_filePath, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        file << "--- Tornado V Enhanced Log Started ---" << std::endl;
        file.close();
    }
}

void Logger::Log(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_filePath.empty()) return; // Don't log if not initialized

    try {
        std::ofstream file(m_filePath, std::ios::out | std::ios::app);
        if (file.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            struct tm timeinfo;
            localtime_s(&timeinfo, &time);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            
            file << "[" << std::put_time(&timeinfo, "%H:%M:%S") << "." 
                 << std::setfill('0') << std::setw(3) << ms.count() << "] " 
                 << message << std::endl;
            file.flush();
            file.close();
        }
    } catch (...) {
        // Silently fail to avoid recursive crashes or deadlocks if logging itself fails
    }
}

void Logger::Error(const std::string& message) {
    Log("[ERROR] " + message);
}
