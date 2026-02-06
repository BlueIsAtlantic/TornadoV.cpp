#pragma once

#include "types.h"
#include <string>
#include <vector>
#include <functional>

enum class MenuItemType {
    Button,
    Checkbox,
    Submenu,
    Int,
    Float
};

struct MenuItem {
    std::string text;
    MenuItemType type;
    bool* checkboxValue;
    int* intValue;
    float* floatValue;
    int minInt, maxInt, stepInt;
    float minFloat, maxFloat, stepFloat;
    int targetSubmenu;
    std::function<void()> action;

    MenuItem(std::string t, std::function<void()> a) 
        : text(t), type(MenuItemType::Button), checkboxValue(nullptr), intValue(nullptr), floatValue(nullptr), targetSubmenu(-1), action(a) {}
    
    MenuItem(std::string t, bool* val, std::function<void()> a = nullptr) 
        : text(t), type(MenuItemType::Checkbox), checkboxValue(val), intValue(nullptr), floatValue(nullptr), targetSubmenu(-1), action(a) {}

    MenuItem(std::string t, int submenu) 
        : text(t), type(MenuItemType::Submenu), checkboxValue(nullptr), intValue(nullptr), floatValue(nullptr), targetSubmenu(submenu), action(nullptr) {}

    MenuItem(std::string t, int* val, int min, int max, int step, std::function<void()> a = nullptr)
        : text(t), type(MenuItemType::Int), checkboxValue(nullptr), intValue(val), floatValue(nullptr), 
          minInt(min), maxInt(max), stepInt(step), targetSubmenu(-1), action(a) {}

    MenuItem(std::string t, float* val, float min, float max, float step, std::function<void()> a = nullptr)
        : text(t), type(MenuItemType::Float), checkboxValue(nullptr), intValue(nullptr), floatValue(val), 
          minFloat(min), maxFloat(max), stepFloat(step), targetSubmenu(-1), action(a) {}
};

struct Submenu {
    std::string title;
    std::string subtitle;
    std::vector<MenuItem> items;
};

class TornadoMenu {
public:
    static void Initialize();
    static void OnTick();
    static void OnKeyDown(DWORD key);
    
    static bool IsVisible() { return m_visible; }
    static void SetVisible(bool visible) { m_visible = visible; }

    // Settings (mirrored from IniHelper/Menu.cs)
    static bool m_movementEnabled;
    static bool m_reverseRotation;
    static bool m_cloudTopEnabled;
    static bool m_cloudTopDebrisEnabled;
    static bool m_surfaceDetectionEnabled;
    static bool m_useInternalPool;
    static bool m_notifications;
    static bool m_spawnInStorm;
    static float m_moveSpeedScale;
    static float m_maxEntityDistance;
    static int m_maxEntityCount;
    static DWORD m_toggleKey;

    // UI Settings
    static float m_menuX;
    static float m_menuY;
    static int m_themeR, m_themeG, m_themeB;
    static int m_bgR, m_bgG, m_bgB, m_bgA;

private:
    static void DrawMenu();
    static void HandleInput();
    static void DrawRect(float x, float y, float width, float height, int r, int g, int b, int a);
    static void DrawText(std::string text, float x, float y, float scale, int font, int r, int g, int b, int a, bool center = false, bool right = false);
    
    static void SetupMenus();
    static bool IsKeyDownWithRepeat(DWORD key);

public:
    static void SpawnTornado();
    static void DespawnTornado();
    static void TeleportToTornado();

private:
    static bool m_visible;
    static int m_currentSubmenu;
    static int m_currentOption;
    static std::vector<Submenu> m_submenus;

    static DWORD m_lastKey;
    static DWORD m_lastRepeatTime;
    static int m_repeatCount;

    static const float MENU_WIDTH;
    static const float TITLE_HEIGHT;
    static const float OPTION_HEIGHT;
    static const int MAX_OPTIONS_VISIBLE;
};
