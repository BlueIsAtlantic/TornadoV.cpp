#include "XmlHelper.h"
#include "tinyxml2/tinyxml2.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include "resource.h"
#include "IniHelper.h" // For ShowNotification

namespace fs = std::filesystem;

std::string XmlHelper::XmlPath = "";

void XmlHelper::Initialize(HMODULE hModule) {
    char path[MAX_PATH];
    GetModuleFileNameA(hModule, path, MAX_PATH);
    fs::path dllPath(path);
    
    // Path: TornadoVStuff\menu_config.xml
    fs::path xmlPath = dllPath.parent_path() / "TornadoVStuff" / "menu_config.xml";
    XmlPath = xmlPath.string();
    
    if (!fs::exists(XmlPath)) {
        DeployDefaultConfig(hModule);
    }
}

void XmlHelper::DeployDefaultConfig(HMODULE hModule) {
    HRSRC hRes = FindResourceA(hModule, MAKEINTRESOURCEA(IDR_XML_DEFAULT), (LPCSTR)RT_RCDATA);
    if (hRes) {
        HGLOBAL hData = LoadResource(hModule, hRes);
        if (hData) {
            DWORD size = SizeofResource(hModule, hRes);
            void* ptr = LockResource(hData);
            
            // Ensure directory exists
            fs::path p(XmlPath);
            fs::create_directories(p.parent_path());

            std::ofstream outFile(XmlPath, std::ios::binary);
            if (outFile.is_open()) {
                outFile.write((const char*)ptr, size);
                outFile.close();
                return;
            }
        }
    }
}

// Since we don't have tinyxml2 yet, these are stubs for now.
// I will implement the real logic once I've added tinyxml2 to the project.
std::string XmlHelper::GetString(const std::string& path, const std::string& defaultValue) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(XmlPath.c_str());
    
    tinyxml2::XMLElement* element = doc.RootElement();
    if (!element) return defaultValue;

    // Split path by dot: MenuConfig.Frame.TitleBox
    std::stringstream ss(path);
    std::string segment;
    std::getline(ss, segment, '.'); // Skip root if it matches
    if (element->name != segment) return defaultValue;

    while (std::getline(ss, segment, '.')) {
        element = element->FirstChildElement(segment.c_str());
        if (!element) return defaultValue;
    }

    const char* val = element->Attribute("value");
    return val ? val : defaultValue;
}

int XmlHelper::GetInt(const std::string& path, int defaultValue) {
    std::string val = GetString(path, "");
    if (val.empty()) return defaultValue;
    return std::stoi(val);
}

float XmlHelper::GetFloat(const std::string& path, float defaultValue) {
    std::string val = GetString(path, "");
    if (val.empty()) return defaultValue;
    return std::stof(val);
}

bool XmlHelper::GetBool(const std::string& path, bool defaultValue) {
    std::string val = GetString(path, "");
    if (val.empty()) return defaultValue;
    return (val == "true" || val == "1");
}

XmlHelper::Color XmlHelper::GetColor(const std::string& path, XmlHelper::Color defaultValue) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(XmlPath.c_str());
    
    tinyxml2::XMLElement* element = doc.RootElement();
    if (!element) return defaultValue;

    std::stringstream ss(path);
    std::string segment;
    std::getline(ss, segment, '.'); // Skip root if it matches
    if (element->name != segment) return defaultValue;

    while (std::getline(ss, segment, '.')) {
        element = element->FirstChildElement(segment.c_str());
        if (!element) return defaultValue;
    }

    XmlHelper::Color result = defaultValue;
    if (element->Attribute("r")) result.r = std::stoi(element->Attribute("r"));
    if (element->Attribute("g")) result.g = std::stoi(element->Attribute("g"));
    if (element->Attribute("b")) result.b = std::stoi(element->Attribute("b"));
    if (element->Attribute("a")) result.a = std::stoi(element->Attribute("a"));
    
    return result;
}

void XmlHelper::WriteValue(const std::string& path, const std::string& value) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(XmlPath.c_str());
    
    tinyxml2::XMLElement* element = doc.RootElement();
    if (!element) return;

    std::stringstream ss(path);
    std::string segment;
    std::getline(ss, segment, '.'); // Skip root if it matches
    if (element->name != segment) return;

    while (std::getline(ss, segment, '.')) {
        tinyxml2::XMLElement* next = element->FirstChildElement(segment.c_str());
        if (!next) {
            next = element->InsertNewChild(segment.c_str());
        }
        element = next;
    }

    element->attributes["value"] = value;
    doc.SaveFile(XmlPath.c_str());
}

void XmlHelper::WriteColor(const std::string& path, XmlHelper::Color value) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(XmlPath.c_str());
    
    tinyxml2::XMLElement* element = doc.RootElement();
    if (!element) return;

    std::stringstream ss(path);
    std::string segment;
    std::getline(ss, segment, '.'); // Skip root if it matches
    if (element->name != segment) return;

    while (std::getline(ss, segment, '.')) {
        tinyxml2::XMLElement* next = element->FirstChildElement(segment.c_str());
        if (!next) {
            next = element->InsertNewChild(segment.c_str());
        }
        element = next;
    }

    element->attributes["r"] = std::to_string(value.r);
    element->attributes["g"] = std::to_string(value.g);
    element->attributes["b"] = std::to_string(value.b);
    element->attributes["a"] = std::to_string(value.a);
    
    doc.SaveFile(XmlPath.c_str());
}
