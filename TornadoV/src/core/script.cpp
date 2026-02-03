#include "script.h"
#include "TornadoFactory.h"
#include "TornadoMenu.h"
#include "IniHelper.h"
#include "MathEx.h"
#include "keyboard.h"
#include "Logger.h"
#include <string>
#include <memory>
#include <filesystem>

std::unique_ptr<TornadoFactory> g_Factory = nullptr;
static HMODULE g_hModule = NULL;

void SetModuleHandle(HMODULE hModule) {
    g_hModule = hModule;
}

void update() {
    static int lastUpdateLog = 0;
    int gameTime = GAMEPLAY::GET_GAME_TIMER();
    
    // Log EVERY update for debugging, but throttle slightly to not spam too much
    static int frameCounter = 0;
    frameCounter++;
    if (frameCounter % 100 == 0) {
        Logger::Log("Update tick " + std::to_string(frameCounter) + " (GameTime: " + std::to_string(gameTime) + ")");
    }

    if (gameTime - lastUpdateLog > 5000) {
        Logger::Log("Update loop heartbeat... (GameTime: " + std::to_string(gameTime) + ")");
        lastUpdateLog = gameTime;
    }

    if (g_Factory) {
        // Logger::Log("Calling Factory::OnUpdate");
        g_Factory->OnUpdate(gameTime);
    }
    
    // Logger::Log("Calling Menu::OnTick");
    TornadoMenu::OnTick();
}

void ModMain() {
    try {
        Logger::Log("ModMain starting...");
        Logger::Log("ModMain: Initializing IniHelper...");
        IniHelper::Initialize(g_hModule);
        
        // Initialize Logger
        std::filesystem::path logPath = std::filesystem::path(IniHelper::GetIniPath()).parent_path() / "TornadoV.log";
        Logger::Initialize(logPath.string());
        Logger::Log("ModMain: Logger initialized at " + logPath.string());

        Logger::Log("ModMain: Initializing MathEx...");
        MathEx::Initialize();
        
        Logger::Log("ModMain: Initializing TornadoMenu...");
        TornadoMenu::Initialize();
        
        Logger::Log("ModMain: Creating TornadoFactory...");
        g_Factory = std::make_unique<TornadoFactory>();
        
        Logger::Log("ModMain: Initialization complete.");
        
        Logger::Log("ModMain: Showing notification...");
        IniHelper::ShowNotification("~g~Tornado V Enhanced Loaded! Press F5 for Menu.");

        Logger::Log("ModMain: Entering main loop...");
        while (true) {
            try {
                // Handle menu toggle key
                if (IsKeyJustUp(TornadoMenu::m_toggleKey)) {
                    Logger::Log("ModMain: Toggle key pressed");
                    TornadoMenu::OnKeyDown(TornadoMenu::m_toggleKey);
                }

                // Handle Tornado toggle key (F6)
                if (IsKeyJustUp(VK_F6)) {
                    Logger::Log("ModMain: F6 pressed");
                    if (g_Factory && g_Factory->GetActiveVortexCount() > 0) {
                        Logger::Log("ModMain: Despawning Tornado...");
                        TornadoMenu::DespawnTornado();
                    } else {
                        Logger::Log("ModMain: Spawning Tornado...");
                        TornadoMenu::SpawnTornado();
                    }
                }

                // Logger::Log("ModMain: Calling update()...");
                update();
            } catch (const std::exception& e) {
                std::string msg = "TornadoV Script Error: ";
                msg += e.what();
                Logger::Error(msg);
                IniHelper::ShowNotification("~r~" + msg);
            } catch (...) {
                Logger::Error("Unknown script error occurred in main loop!");
                IniHelper::ShowNotification("~r~TornadoV: Unknown script error occurred!");
            }
            
            WAIT(0);
        }
    } catch (const std::exception& e) {
        std::string msg = "TornadoV Fatal Init Error: ";
        msg += e.what();
        Logger::Error(msg);
    } catch (...) {
        Logger::Error("TornadoV Fatal Unknown Init Error!");
    }
}

void ScriptMain() {
    srand(static_cast<unsigned int>(GAMEPLAY::GET_GAME_TIMER()));
    ModMain();
}
