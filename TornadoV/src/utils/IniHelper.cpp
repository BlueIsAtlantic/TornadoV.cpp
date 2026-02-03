#include "IniHelper.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include "resource.h"
#include "main.h"
#include "natives.h"

namespace fs = std::filesystem;

std::string IniHelper::IniPath = "";

void IniHelper::Initialize(HMODULE hModule) {
    char path[MAX_PATH];
    GetModuleFileNameA(hModule, path, MAX_PATH);
    fs::path dllPath(path);
    
    // Set the INI path to be in the same directory as the ASI, with the same name
    fs::path iniPath = dllPath.parent_path() / dllPath.stem();
    iniPath.replace_extension(".ini");
    
    IniPath = iniPath.string();
    
    if (!fs::exists(IniPath)) {
        DeployDefaultConfig(hModule);
    }
}

void IniHelper::DeployDefaultConfig(HMODULE hModule) {
    HRSRC hRes = FindResourceA(hModule, MAKEINTRESOURCEA(IDR_INI_DEFAULT), (LPCSTR)RT_RCDATA);
    if (hRes) {
        HGLOBAL hData = LoadResource(hModule, hRes);
        if (hData) {
            DWORD size = SizeofResource(hModule, hRes);
            void* ptr = LockResource(hData);
            
            std::ofstream outFile(IniPath, std::ios::binary);
            if (outFile.is_open()) {
                outFile.write((const char*)ptr, size);
                outFile.close();
                ShowNotification("~g~Tornado V: Default configuration deployed!");
                return;
            }
        }
    }
    
    // Fallback if resource extraction fails
    ShowNotification("~r~Tornado V: Failed to deploy default configuration!");
}

void IniHelper::WriteValue(const std::string& section, const std::string& key, const std::string& value) {
    WritePrivateProfileStringA(section.c_str(), key.c_str(), value.c_str(), IniPath.c_str());
}

void IniHelper::ShowNotification(const std::string& message) {
    UI::_SET_NOTIFICATION_TEXT_ENTRY(const_cast<char*>("STRING"));
    UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(message.c_str()));
    UI::_DRAW_NOTIFICATION(false, true);
}
