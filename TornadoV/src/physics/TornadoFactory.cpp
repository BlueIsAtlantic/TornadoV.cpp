#include "TornadoFactory.h"
#include "TornadoMenu.h"
#include "natives.h"
#include "MathEx.h"
#include "IniHelper.h"
#include "Logger.h"
#include <algorithm>
#include <cmath>

TornadoFactory::TornadoFactory()
    : m_spawnDelayAdditive(0), m_spawnDelayStartTime(0),
      m_lastSpawnAttempt(0), m_lastSpawnCompleteTime(0),
      m_spawnInProgress(false), m_delaySpawn(false) {
}

TornadoFactory::~TornadoFactory() {
    Dispose();
}

TornadoVortex* TornadoFactory::CreateVortex(Vector3 position) {
    int gameTime = GAMEPLAY::GET_GAME_TIMER();
    Logger::Log("Factory: CreateVortex at (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
    
    // OPTIMIZATION: Enforce cooldown between spawns
    if (gameTime - m_lastSpawnCompleteTime < SPAWN_COOLDOWN) {
        Logger::Log("Factory: Spawn rejected (cooldown)");
        return nullptr;
    }
    if (m_spawnInProgress) {
        Logger::Log("Factory: Spawn rejected (already in progress)");
        return nullptr;
    }
    if (std::isnan(position.x) || std::isnan(position.y) || std::isnan(position.z)) return nullptr;

    m_spawnInProgress = true; // Set BEFORE any operations

    // Maintain limit by removing oldest if necessary
    if (m_activeVortexList.size() >= VortexLimit) {
        m_activeVortexList.front()->Dispose();
        m_activeVortexList.erase(m_activeVortexList.begin());
    }

    float groundZ;
    GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(position.x, position.y, 1000.0f, &groundZ, false);
    
    if (groundZ < -1000.0f || std::isnan(groundZ)) {
        groundZ = position.z;
    }

    position.z = groundZ - 10.0f;

    auto tVortex = std::make_unique<TornadoVortex>(position, false);

    try {
        // OPTIMIZATION: Clear old particles before building new ones
        GRAPHICS::REMOVE_PARTICLE_FX_IN_RANGE(position.x, position.y, position.z, 200.0f);

        // Wait one frame to let particles clear
        WAIT(0);

        Logger::Log("Factory: Calling Build()...");
        tVortex->Build();
        Logger::Log("Factory: Build() returned.");

        // OPTIMIZATION: Wait another frame after building
        WAIT(0);
    }
    catch (const std::exception& e) {
        Logger::Error("Factory: Error during Build: " + std::string(e.what()));
        m_spawnInProgress = false;
        return nullptr;
    }
    catch (...) {
        Logger::Error("Factory: Unknown error during Build");
        m_spawnInProgress = false;
        return nullptr;
    }

    TornadoVortex* ptr = tVortex.get();
    m_activeVortexList.push_back(std::move(tVortex));

    // Notification
    if (TornadoMenu::m_notifications) {
        IniHelper::ShowNotification("~g~Tornado spawned nearby.");
    }

    m_lastSpawnCompleteTime = GAMEPLAY::GET_GAME_TIMER(); // OPTIMIZATION: Track completion time
    m_spawnInProgress = false; // Reset flag

    return ptr;
}

void TornadoFactory::OnUpdate(int gameTime) {
    if (m_activeVortexList.empty()) {
        if (TornadoMenu::m_spawnInStorm) {
                bool isStorming = GAMEPLAY::GET_RAIN_LEVEL() > 0.1f;

                if (isStorming && !m_spawnInProgress && gameTime - m_lastSpawnAttempt > 1000) {
                    if ((float)rand() / RAND_MAX < 0.05f) {
                        m_spawnDelayStartTime = gameTime;
                        m_spawnDelayAdditive = rand() % 40000;

                        GAMEPLAY::SET_WIND_SPEED(70.0f);
                        m_spawnInProgress = true;
                    }
                    m_lastSpawnAttempt = gameTime;
                }
        }
    }

    if (m_spawnInProgress && m_spawnDelayStartTime != 0) {
        if (gameTime - m_spawnDelayStartTime > m_spawnDelayAdditive) {
            Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
            float angle = (float)rand() / RAND_MAX * 6.28318f;
            float dist = 200.0f + (float)rand() / RAND_MAX * 200.0f;
            playerPos.x += std::cos(angle) * dist;
            playerPos.y += std::sin(angle) * dist;

            CreateVortex(playerPos);
            m_spawnDelayStartTime = 0;
            m_spawnDelayAdditive = 0;
        }
    }

    for (auto it = m_activeVortexList.begin(); it != m_activeVortexList.end();) {
        if ((*it)->DespawnRequested) {
            (*it)->Dispose();
            it = m_activeVortexList.erase(it);
        } else {
            (*it)->OnUpdate(gameTime);
            ++it;
        }
    }
}

void TornadoFactory::RemoveAll() {
    m_spawnInProgress = false;

    for (auto& vortex : m_activeVortexList) {
        vortex->Dispose();
    }
    m_activeVortexList.clear();

    Ped playerPed = PLAYER::PLAYER_PED_ID();
    Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(playerPed, true);
    GRAPHICS::REMOVE_PARTICLE_FX_IN_RANGE(playerPos.x, playerPos.y, playerPos.z, 1000.0f);
}

void TornadoFactory::Dispose() {
    RemoveAll();
}
