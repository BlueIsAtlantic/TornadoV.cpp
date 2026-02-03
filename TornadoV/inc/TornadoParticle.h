#pragma once
#include "types.h"
#include "MathEx.h"
#include "LoopedParticle.h"
#include <memory>

class TornadoVortex;

class TornadoParticle {
public:
    TornadoParticle(TornadoVortex* vortex, Vector3 position, Vector3 angle, 
                   const std::string& fxAsset, const std::string& fxName, 
                   float radius, int layerIdx, bool isCloud = false);
    ~TornadoParticle();

    void OnUpdate(int gameTime);
    void StartFx(float scale);
    void RemoveFx();
    void Dispose();

    Entity Ref; // In C# this is 'Ref' from ScriptEntity
    int LayerIndex;
    TornadoVortex* Parent;
    bool IsCloud;

private:
    static Entity SafeSetup(Vector3 position);
    void PostSetup();
    void RefreshCache();

    Vector3 _centerPos;
    Vector3 _offset;
    Quaternion _rotation;
    std::unique_ptr<LoopedParticle> _ptfx;
    float _radius;
    float _angle;
    float _layerMask;

    float _cachedRotationSpeed;
    float _cachedLayerSeparation;
    int _lastCacheTime;

    int _updateSkipCounter;
    static const int UPDATE_SKIP_FREQUENCY = 2;
};
