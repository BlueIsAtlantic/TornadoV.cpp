#pragma once
#include "soloud.h"
#include "soloud_wav.h"
#include <string>
#include <map>
#include <vector>

class AudioManager {
public:
    static AudioManager& Get();

    void Init();
    void Deinit();

    // 2D sounds (UI, Notifications)
    unsigned int Play2D(const std::string& name, float volume = 1.0f, bool loop = false);
    
    // 3D sounds (Tornado, Sirens)
    unsigned int Play3D(const std::string& name, float x, float y, float z, float volume = 1.0f, bool loop = false);

    void UpdateListener(float x, float y, float z, float lookX, float lookY, float lookZ, float upX = 0, float upY = 0, float upZ = 1);
    void Update3DSound(unsigned int handle, float x, float y, float z);
    void SetVolume(unsigned int handle, float volume);
    void Stop(unsigned int handle);
    void StopAll();

    bool LoadSound(const std::string& name, const std::string& path);

private:
    AudioManager() = default;
    ~AudioManager();

    SoLoud::Soloud m_soloud;
    std::map<std::string, SoLoud::Wav*> m_sounds;
};
