#pragma once
#include <string>
#include "types.h"
#include "natives.h"

class LoopedParticle {
public:
    LoopedParticle(const std::string& assetName, const std::string& fxName);
    ~LoopedParticle();

    void Load();
    bool IsLoaded() const;
    bool Exists() const;

    void Start(Entity entity, float scale, Vector3 offset = { 0,0,0,0,0,0 }, Vector3 rotation = { 0,0,0,0,0,0 }, int bone = -1);
    void Start(Vector3 position, float scale, Vector3 rotation = { 0,0,0,0,0,0 });

    void Remove();
    void Remove(Vector3 position, float radius);
    void Unload();

    void SetAlpha(float alpha);
    void SetScale(float scale);
    void SetColour(float r, float g, float b, float a = 0.0f);
    void SetOffsets(Vector3 offset, Vector3 rotOffset);
    void SetEvolution(const std::string& variableName, float value);

    float GetAlpha() const { return m_alpha; }
    float GetScale() const { return m_scale; }
    int GetHandle() const { return m_handle; }
    std::string GetAssetName() const { return m_assetName; }

private:
    std::string m_assetName;
    std::string m_fxName;
    int m_handle;
    float m_scale;
    float m_alpha;
};
