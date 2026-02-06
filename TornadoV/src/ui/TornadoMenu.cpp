#include "TornadoMenu.h"
#include "natives.h"
#include "IniHelper.h"
#include "TornadoFactory.h"
#include "Logger.h"
#include "MathEx.h"
#include "script.h"
#include "keyboard.h"
#include <cmath>
#include <memory>
#include <cstdio>

extern std::unique_ptr<TornadoFactory> g_Factory;

bool TornadoMenu::m_visible = false;
int TornadoMenu::m_currentSubmenu = 0;
int TornadoMenu::m_currentOption = 0;
std::vector<Submenu> TornadoMenu::m_submenus;
DWORD TornadoMenu::m_lastKey = 0;
DWORD TornadoMenu::m_lastRepeatTime = 0;
int TornadoMenu::m_repeatCount = 0;

bool TornadoMenu::m_movementEnabled = true;
bool TornadoMenu::m_reverseRotation = false;
bool TornadoMenu::m_cloudTopEnabled = false;
bool TornadoMenu::m_cloudTopDebrisEnabled = false;
bool TornadoMenu::m_surfaceDetectionEnabled = true;
bool TornadoMenu::m_useInternalPool = true;
bool TornadoMenu::m_notifications = true;
bool TornadoMenu::m_spawnInStorm = true;
float TornadoMenu::m_moveSpeedScale = 1.0f;
float TornadoMenu::m_maxEntityDistance = 57.0f;
int TornadoMenu::m_maxEntityCount = 200;
DWORD TornadoMenu::m_toggleKey = VK_F5;

float TornadoMenu::m_menuX = 0.15f;
float TornadoMenu::m_menuY = 0.1f;
int TornadoMenu::m_themeR = 184;
int TornadoMenu::m_themeG = 162;
int TornadoMenu::m_themeB = 57;
int TornadoMenu::m_bgR = 0;
int TornadoMenu::m_bgG = 0;
int TornadoMenu::m_bgB = 0;
int TornadoMenu::m_bgA = 200;

const float TornadoMenu::MENU_WIDTH = 0.21f;
const float TornadoMenu::TITLE_HEIGHT = 0.08f;
const float TornadoMenu::OPTION_HEIGHT = 0.035f;
const int TornadoMenu::MAX_OPTIONS_VISIBLE = 10;

void TornadoMenu::Initialize() {
    m_movementEnabled = IniHelper::GetValue("Vortex", "MovementEnabled", true);
    m_reverseRotation = IniHelper::GetValue("Vortex", "ReverseRotation", false);
    m_cloudTopEnabled = IniHelper::GetValue("VortexAdvanced", "CloudTopEnabled", false);
    m_cloudTopDebrisEnabled = IniHelper::GetValue("VortexAdvanced", "CloudTopDebrisEnabled", false);
    m_surfaceDetectionEnabled = IniHelper::GetValue("VortexAdvanced", "SurfaceDetectionEnabled", true);
    m_useInternalPool = IniHelper::GetValue("VortexAdvanced", "UseInternalPool", true);
    m_notifications = IniHelper::GetValue("Other", "Notifications", true);
    m_spawnInStorm = IniHelper::GetValue("Other", "SpawnInStorm", true);
    m_moveSpeedScale = IniHelper::GetValue("Vortex", "MoveSpeedScale", 1.0f);
    m_maxEntityDistance = IniHelper::GetValue("Vortex", "MaxEntityDistance", 57.0f);
    m_maxEntityCount = IniHelper::GetValue("Vortex", "MaxEntityCount", 200);
    
    // UI Settings
    m_menuX = IniHelper::GetValue("Menu", "X", 0.15f);
    m_menuY = IniHelper::GetValue("Menu", "Y", 0.1f);
    m_themeR = IniHelper::GetValue("Menu", "ThemeR", 184);
    m_themeG = IniHelper::GetValue("Menu", "ThemeG", 162);
    m_themeB = IniHelper::GetValue("Menu", "ThemeB", 57);
    m_bgR = IniHelper::GetValue("Menu", "BackgroundR", 0);
    m_bgG = IniHelper::GetValue("Menu", "BackgroundG", 0);
    m_bgB = IniHelper::GetValue("Menu", "BackgroundB", 0);
    m_bgA = IniHelper::GetValue("Menu", "BackgroundA", 200);

    // Keybinds
    std::string keyStr = IniHelper::GetValue("KeyBinds", "ToggleMenu", "F5");
    if (keyStr == "F5") m_toggleKey = VK_F5;
    else if (keyStr == "F6") m_toggleKey = VK_F6;
    else if (keyStr == "F7") m_toggleKey = VK_F7;
    else if (keyStr == "F8") m_toggleKey = VK_F8;

    SetupMenus();
}

void TornadoMenu::SetupMenus() {
    m_submenus.clear();
    
    int screenW, screenH;
    GRAPHICS::_GET_SCREEN_ACTIVE_RESOLUTION(&screenW, &screenH);
    float pixelX = 1.0f / (float)screenW;
    float pixelY = 1.0f / (float)screenH;

    // Main Menu (Index 0)
    Submenu main;
    main.title = "TornadoV++";
    main.subtitle = "Control Panel";
    main.items.push_back(MenuItem("Spawn Tornado", []() { SpawnTornado(); }));
    main.items.push_back(MenuItem("Despawn Tornado", []() { DespawnTornado(); }));
    main.items.push_back(MenuItem("Teleport to Tornado", []() { TeleportToTornado(); }));
    main.items.push_back(MenuItem("Settings", 1)); // Points to submenu index 1
    main.items.push_back(MenuItem("Menu Settings", 2)); // Points to submenu index 2
    m_submenus.push_back(main);

    // Settings Menu (Index 1)
    Submenu settings;
    settings.title = "TornadoV++";
    settings.subtitle = "Settings";
    settings.items.push_back(MenuItem("Movement Enabled", &m_movementEnabled, []() {
        IniHelper::WriteValue("Vortex", "MovementEnabled", m_movementEnabled ? "true" : "false");
    }));
    settings.items.push_back(MenuItem("Reverse Rotation", &m_reverseRotation, []() {
        IniHelper::WriteValue("Vortex", "ReverseRotation", m_reverseRotation ? "true" : "false");
    }));
    settings.items.push_back(MenuItem("Cloud Top Enabled", &m_cloudTopEnabled, []() {
        IniHelper::WriteValue("VortexAdvanced", "CloudTopEnabled", m_cloudTopEnabled ? "true" : "false");
    }));
    settings.items.push_back(MenuItem("Cloud Top Debris", &m_cloudTopDebrisEnabled, []() {
        IniHelper::WriteValue("VortexAdvanced", "CloudTopDebrisEnabled", m_cloudTopDebrisEnabled ? "true" : "false");
    }));
    settings.items.push_back(MenuItem("Surface Detection", &m_surfaceDetectionEnabled, []() {
        IniHelper::WriteValue("VortexAdvanced", "SurfaceDetectionEnabled", m_surfaceDetectionEnabled ? "true" : "false");
    }));
    settings.items.push_back(MenuItem("Use Internal Pool", &m_useInternalPool, []() {
        IniHelper::WriteValue("VortexAdvanced", "UseInternalPool", m_useInternalPool ? "true" : "false");
    }));
    settings.items.push_back(MenuItem("Notifications", &m_notifications, []() {
        IniHelper::WriteValue("Other", "Notifications", m_notifications ? "true" : "false");
    }));
    settings.items.push_back(MenuItem("Spawn In Storm", &m_spawnInStorm, []() {
        IniHelper::WriteValue("Other", "SpawnInStorm", m_spawnInStorm ? "true" : "false");
    }));
    settings.items.push_back(MenuItem("Max Entity Distance", &m_maxEntityDistance, 10.0f, 500.0f, 5.0f, []() {
        IniHelper::WriteValue("Vortex", "MaxEntityDistance", std::to_string(m_maxEntityDistance));
    }));
    settings.items.push_back(MenuItem("Max Entity Count", &m_maxEntityCount, 10, 1000, 10, []() {
        IniHelper::WriteValue("Vortex", "MaxEntityCount", std::to_string(m_maxEntityCount));
    }));
    m_submenus.push_back(settings);

    // Menu Settings (Index 2)
    Submenu menuSettings;
    menuSettings.title = "TornadoV++";
    menuSettings.subtitle = "Menu Settings";
    
    // PIXEL BY PIXEL adjustment with clamping to avoid screen edge clipping
    float minX = MENU_WIDTH / 2.0f;
    float maxX = 1.0f - (MENU_WIDTH / 2.0f);
    menuSettings.items.push_back(MenuItem("Menu X", &m_menuX, minX, maxX, pixelX, []() {
        IniHelper::WriteValue("Menu", "X", std::to_string(m_menuX));
    }));
    menuSettings.items.push_back(MenuItem("Menu Y", &m_menuY, 0.05f, 0.95f, pixelY, []() {
        IniHelper::WriteValue("Menu", "Y", std::to_string(m_menuY));
    }));
    
    menuSettings.items.push_back(MenuItem("Theme Red", &m_themeR, 0, 255, 5, []() {
        IniHelper::WriteValue("Menu", "ThemeR", std::to_string(m_themeR));
    }));
    menuSettings.items.push_back(MenuItem("Theme Green", &m_themeG, 0, 255, 5, []() {
        IniHelper::WriteValue("Menu", "ThemeG", std::to_string(m_themeG));
    }));
    menuSettings.items.push_back(MenuItem("Theme Blue", &m_themeB, 0, 255, 5, []() {
        IniHelper::WriteValue("Menu", "ThemeB", std::to_string(m_themeB));
    }));
    
    menuSettings.items.push_back(MenuItem("Background Red", &m_bgR, 0, 255, 5, []() {
        IniHelper::WriteValue("Menu", "BackgroundR", std::to_string(m_bgR));
    }));
    menuSettings.items.push_back(MenuItem("Background Green", &m_bgG, 0, 255, 5, []() {
        IniHelper::WriteValue("Menu", "BackgroundG", std::to_string(m_bgG));
    }));
    menuSettings.items.push_back(MenuItem("Background Blue", &m_bgB, 0, 255, 5, []() {
        IniHelper::WriteValue("Menu", "BackgroundB", std::to_string(m_bgB));
    }));
    menuSettings.items.push_back(MenuItem("Background Opacity", &m_bgA, 0, 255, 5, []() {
        IniHelper::WriteValue("Menu", "BackgroundA", std::to_string(m_bgA));
    }));
    
    m_submenus.push_back(menuSettings);
}

void TornadoMenu::OnTick() {
    if (m_visible) {
        // Disable game controls when menu is open
        CONTROLS::DISABLE_CONTROL_ACTION(0, 27, true); // INPUT_PHONE
        CONTROLS::DISABLE_CONTROL_ACTION(0, 19, true); // INPUT_CHARACTER_WHEEL
        CONTROLS::DISABLE_CONTROL_ACTION(0, 140, true); // INPUT_MELEE_ATTACK_LIGHT
        CONTROLS::DISABLE_CONTROL_ACTION(0, 141, true); // INPUT_MELEE_ATTACK_HEAVY
        CONTROLS::DISABLE_CONTROL_ACTION(0, 142, true); // INPUT_MELEE_ATTACK_ALTERNATE
        CONTROLS::DISABLE_CONTROL_ACTION(0, 257, true); // INPUT_ATTACK2
        CONTROLS::DISABLE_CONTROL_ACTION(0, 263, true); // INPUT_MELEE_ATTACK1
        CONTROLS::DISABLE_CONTROL_ACTION(0, 264, true); // INPUT_MELEE_ATTACK2
        CONTROLS::DISABLE_CONTROL_ACTION(0, 24, true); // INPUT_ATTACK
        CONTROLS::DISABLE_CONTROL_ACTION(0, 25, true); // INPUT_AIM
        CONTROLS::DISABLE_CONTROL_ACTION(0, 14, true); // INPUT_WEAPON_WHEEL_NEXT
        CONTROLS::DISABLE_CONTROL_ACTION(0, 15, true); // INPUT_WEAPON_WHEEL_PREV
        CONTROLS::DISABLE_CONTROL_ACTION(0, 16, true); // INPUT_SELECT_NEXT_WEAPON
        CONTROLS::DISABLE_CONTROL_ACTION(0, 17, true); // INPUT_SELECT_PREV_WEAPON
        
        // Disable arrow key game actions
        CONTROLS::DISABLE_CONTROL_ACTION(0, 172, true); // INPUT_CELLPHONE_UP
        CONTROLS::DISABLE_CONTROL_ACTION(0, 173, true); // INPUT_CELLPHONE_DOWN
        CONTROLS::DISABLE_CONTROL_ACTION(0, 174, true); // INPUT_CELLPHONE_LEFT
        CONTROLS::DISABLE_CONTROL_ACTION(0, 175, true); // INPUT_CELLPHONE_RIGHT
        
        DrawMenu();
        HandleInput();
    }
}

void TornadoMenu::OnKeyDown(DWORD key) {
    if (key == m_toggleKey) {
        m_visible = !m_visible;
        if (m_visible) {
            m_currentSubmenu = 0;
            m_currentOption = 0;
            AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("SELECT"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
        } else {
            AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("QUIT"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
        }
    }
}

void TornadoMenu::DrawMenu() {
    if (m_currentSubmenu >= m_submenus.size()) return;

    // Clamp menu position to stay on screen
    float minX = MENU_WIDTH / 2.0f;
    float maxX = 1.0f - (MENU_WIDTH / 2.0f);
    if (m_menuX < minX) m_menuX = minX;
    if (m_menuX > maxX) m_menuX = maxX;

    const Submenu& current = m_submenus[m_currentSubmenu];
    float y = m_menuY;

    // Draw Title
    DrawRect(m_menuX, y + TITLE_HEIGHT / 2, MENU_WIDTH, TITLE_HEIGHT, m_themeR, m_themeG, m_themeB, 255);
    DrawText(current.title, m_menuX, y + TITLE_HEIGHT / 2 - 0.02f, 0.8f, 1, 255, 255, 255, 255, true);
    y += TITLE_HEIGHT;

    // Draw Subtitle
    DrawRect(m_menuX, y + OPTION_HEIGHT / 2, MENU_WIDTH, OPTION_HEIGHT, 0, 0, 0, 255);
    DrawText(current.subtitle, m_menuX - MENU_WIDTH / 2 + 0.005f, y + OPTION_HEIGHT / 2 - 0.015f, 0.35f, 0, m_themeR, m_themeG, m_themeB, 255, false);
    DrawText(std::to_string(m_currentOption + 1) + " / " + std::to_string(current.items.size()), m_menuX + MENU_WIDTH / 2 - 0.005f, y + OPTION_HEIGHT / 2 - 0.015f, 0.35f, 0, m_themeR, m_themeG, m_themeB, 255, false, true);
    y += OPTION_HEIGHT;

    // Draw Options
    for (int i = 0; i < current.items.size(); ++i) {
        const MenuItem& item = current.items[i];
        bool isSelected = (i == m_currentOption);

        int r = isSelected ? 255 : m_bgR;
        int g = isSelected ? 255 : m_bgG;
        int b = isSelected ? 255 : m_bgB;

        DrawRect(m_menuX, y + OPTION_HEIGHT / 2, MENU_WIDTH, OPTION_HEIGHT, r, g, b, isSelected ? 255 : m_bgA);
        DrawText(item.text, m_menuX - MENU_WIDTH / 2 + 0.005f, y + OPTION_HEIGHT / 2 - 0.015f, 0.35f, 0, isSelected ? 0 : 255, isSelected ? 0 : 255, isSelected ? 0 : 255, 255, false);

        if (item.type == MenuItemType::Checkbox) {
            // Menyoo-style toggle squares
            float squareSize = 0.012f;
            float squareX = m_menuX + MENU_WIDTH / 2 - 0.015f;
            float squareY = y + OPTION_HEIGHT / 2;
            
            // Background of toggle
            DrawRect(squareX, squareY, squareSize, squareSize * 1.8f, 50, 50, 50, 255);
            
            // Toggle color
            if (*item.checkboxValue) {
                DrawRect(squareX, squareY, squareSize - 0.004f, (squareSize - 0.004f) * 1.8f, 0, 200, 0, 255); // Green
            } else {
                DrawRect(squareX, squareY, squareSize - 0.004f, (squareSize - 0.004f) * 1.8f, 200, 0, 0, 255); // Red
            }
        } else if (item.type == MenuItemType::Submenu) {
            DrawText(">", m_menuX + MENU_WIDTH / 2 - 0.01f, y + OPTION_HEIGHT / 2 - 0.015f, 0.35f, 0, isSelected ? 0 : 255, isSelected ? 0 : 255, isSelected ? 0 : 255, 255, false, true);
        } else if (item.type == MenuItemType::Int) {
            DrawText("< " + std::to_string(*item.intValue) + " >", m_menuX + MENU_WIDTH / 2 - 0.005f, y + OPTION_HEIGHT / 2 - 0.015f, 0.35f, 0, isSelected ? 0 : 255, isSelected ? 0 : 255, isSelected ? 0 : 255, 255, false, true);
        } else if (item.type == MenuItemType::Float) {
            char buf[32];
            sprintf_s(buf, "< %.2f >", *item.floatValue);
            DrawText(buf, m_menuX + MENU_WIDTH / 2 - 0.005f, y + OPTION_HEIGHT / 2 - 0.015f, 0.35f, 0, isSelected ? 0 : 255, isSelected ? 0 : 255, isSelected ? 0 : 255, 255, false, true);
        }

        y += OPTION_HEIGHT;
    }
}

bool TornadoMenu::IsKeyDownWithRepeat(DWORD key) {
    if (IsKeyDown(key)) {
        if (m_lastKey != key) {
            m_lastKey = key;
            m_lastRepeatTime = GetTickCount();
            m_repeatCount = 0;
            return true;
        } else {
            DWORD currentTime = GetTickCount();
            // Initial delay of 500ms, then repeats every 50ms (getting faster)
            DWORD delay = (m_repeatCount == 0) ? 500 : (std::max)(10, 50 - (m_repeatCount * 2));
            if (currentTime - m_lastRepeatTime > delay) {
                m_lastRepeatTime = currentTime;
                m_repeatCount++;
                return true;
            }
        }
    } else if (m_lastKey == key) {
        m_lastKey = 0;
        m_repeatCount = 0;
    }
    return false;
}

void TornadoMenu::HandleInput() {
    const Submenu& current = m_submenus[m_currentSubmenu];

    if (IsKeyDownWithRepeat(VK_NUMPAD8) || IsKeyDownWithRepeat(VK_UP)) {
        m_currentOption--;
        if (m_currentOption < 0) m_currentOption = (int)current.items.size() - 1;
        AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("NAV_UP_DOWN"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
    } else if (IsKeyDownWithRepeat(VK_NUMPAD2) || IsKeyDownWithRepeat(VK_DOWN)) {
        m_currentOption++;
        if (m_currentOption >= current.items.size()) m_currentOption = 0;
        AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("NAV_UP_DOWN"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
    } else if (IsKeyJustUp(VK_NUMPAD5) || IsKeyJustUp(VK_RETURN)) {
        const MenuItem& item = current.items[m_currentOption];
        if (item.type == MenuItemType::Button) {
            if (item.action) item.action();
        } else if (item.type == MenuItemType::Checkbox) {
            if (item.checkboxValue) {
                *item.checkboxValue = !(*item.checkboxValue);
                if (item.action) item.action();
            }
        } else if (item.type == MenuItemType::Submenu) {
            m_currentSubmenu = item.targetSubmenu;
            m_currentOption = 0;
        }
        AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("SELECT"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
    } else if (IsKeyJustUp(VK_NUMPAD0) || IsKeyJustUp(VK_BACK)) {
        if (m_currentSubmenu > 0) {
            m_currentSubmenu = 0; // Go back to main menu
            m_currentOption = 0;
            AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("BACK"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
        } else {
            m_visible = false;
            AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("QUIT"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
        }
    } else if (IsKeyDownWithRepeat(VK_NUMPAD4) || IsKeyDownWithRepeat(VK_LEFT)) {
        const MenuItem& item = current.items[m_currentOption];
        if (item.type == MenuItemType::Int) {
            *item.intValue -= item.stepInt;
            if (*item.intValue < item.minInt) *item.intValue = item.maxInt;
            if (item.action) item.action();
            AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("NAV_UP_DOWN"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
        } else if (item.type == MenuItemType::Float) {
            *item.floatValue -= item.stepFloat;
            if (*item.floatValue < item.minFloat) *item.floatValue = item.maxFloat;
            if (item.action) item.action();
            AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("NAV_UP_DOWN"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
        }
    } else if (IsKeyDownWithRepeat(VK_NUMPAD6) || IsKeyDownWithRepeat(VK_RIGHT)) {
        const MenuItem& item = current.items[m_currentOption];
        if (item.type == MenuItemType::Int) {
            *item.intValue += item.stepInt;
            if (*item.intValue > item.maxInt) *item.intValue = item.minInt;
            if (item.action) item.action();
            AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("NAV_UP_DOWN"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
        } else if (item.type == MenuItemType::Float) {
            *item.floatValue += item.stepFloat;
            if (*item.floatValue > item.maxFloat) *item.floatValue = item.minFloat;
            if (item.action) item.action();
            AUDIO::PLAY_SOUND_FRONTEND(-1, const_cast<char*>("NAV_UP_DOWN"), const_cast<char*>("HUD_FRONTEND_DEFAULT_SOUNDSET"), true);
        }
    }
}

void TornadoMenu::DrawRect(float x, float y, float width, float height, int r, int g, int b, int a) {
    GRAPHICS::DRAW_RECT(x, y, width, height, r, g, b, a);
}

void TornadoMenu::DrawText(std::string text, float x, float y, float scale, int font, int r, int g, int b, int a, bool center, bool right) {
    UI::SET_TEXT_FONT(font);
    UI::SET_TEXT_SCALE(0.0f, scale);
    UI::SET_TEXT_COLOUR(r, g, b, a);
    UI::SET_TEXT_CENTRE(center);
    if (right) {
        UI::SET_TEXT_RIGHT_JUSTIFY(true);
        UI::SET_TEXT_WRAP(0.0f, x);
    }
    UI::_SET_TEXT_ENTRY(const_cast<char*>("STRING"));
    UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(text.c_str()));
    UI::_DRAW_TEXT(x, y);
}

void TornadoMenu::SpawnTornado() {
    Logger::Log("Menu: SpawnTornado starting...");
    if (!g_Factory) {
        Logger::Log("Menu: g_Factory is null!");
        return;
    }

    if (g_Factory->GetActiveVortexCount() > 0) {
        Logger::Log("Menu: Already a tornado active, multi-vortex disabled.");
        return;
    }

    // Original C# logic: remove particles in range and set wind
    GRAPHICS::REMOVE_PARTICLE_FX_IN_RANGE(0.0f, 0.0f, 0.0f, 500.0f);
    if (m_spawnInStorm) {
        GAMEPLAY::SET_WIND(70.0f);
    }

    Ped playerPed = PLAYER::PLAYER_PED_ID();
    Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(playerPed, true);
    Vector3 forward = ENTITY::GET_ENTITY_FORWARD_VECTOR(playerPed);
    
    Vector3 spawnPos = MathEx::Add(playerPos, MathEx::Multiply(forward, 180.0f));
    Logger::Log("Menu: Calling CreateVortex...");
    g_Factory->CreateVortex(spawnPos);
    Logger::Log("Menu: CreateVortex returned.");
    
    if (m_notifications) {
        IniHelper::ShowNotification("Tornado spawned!");
    }
}

void TornadoMenu::DespawnTornado() {
    if (!g_Factory) return;
    g_Factory->RemoveAll();
    if (m_notifications) {
        IniHelper::ShowNotification("All tornadoes despawned!");
    }
}

void TornadoMenu::TeleportToTornado() {
    if (!g_Factory || g_Factory->GetActiveVortexCount() == 0) {
        if (m_notifications) {
            IniHelper::ShowNotification("No active tornado found.");
        }
        return;
    }

    TornadoVortex* vortex = g_Factory->GetFirstVortex();
    if (vortex) {
        Vector3 pos = vortex->GetPosition();
        Ped playerPed = PLAYER::PLAYER_PED_ID();
        
        // Teleport player slightly above ground at tornado position
        ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z + 10.0f, false, false, false);
        
        if (m_notifications) {
            IniHelper::ShowNotification("Teleported to tornado.");
        }
    }
}
