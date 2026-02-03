#pragma once
#include <vector>
#include <memory>
#include "types.h"
#include "TornadoVortex.h"

class TornadoFactory {
public:
    TornadoFactory();
    ~TornadoFactory();

    TornadoVortex* CreateVortex(Vector3 position);
    void OnUpdate(int gameTime);
    void RemoveAll();
    void Dispose();

    int GetActiveVortexCount() const { return (int)m_activeVortexList.size(); }
    TornadoVortex* GetFirstVortex() { return m_activeVortexList.empty() ? nullptr : m_activeVortexList.front().get(); }

private:
    static const int VortexLimit = 30;
    static const int TornadoSpawnDelayBase = 20000;
    static const int SPAWN_COOLDOWN = 2000;

    std::vector<std::unique_ptr<TornadoVortex>> m_activeVortexList;
    
    int m_spawnDelayAdditive;
    int m_spawnDelayStartTime;
    int m_lastSpawnAttempt;
    int m_lastSpawnCompleteTime;
    
    bool m_spawnInProgress;
    bool m_delaySpawn;
};

extern std::unique_ptr<TornadoFactory> g_Factory;
