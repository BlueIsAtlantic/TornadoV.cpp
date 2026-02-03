#include "LoopedParticle.h"

LoopedParticle::LoopedParticle(const std::string& assetName, const std::string& fxName)
    : m_assetName(assetName), m_fxName(fxName), m_handle(-1), m_scale(1.0f), m_alpha(1.0f) {
}

LoopedParticle::~LoopedParticle() {
    Remove();
}

void LoopedParticle::Load() {
    STREAMING::REQUEST_NAMED_PTFX_ASSET(const_cast<char*>(m_assetName.c_str()));
}

bool LoopedParticle::IsLoaded() const {
    return STREAMING::HAS_NAMED_PTFX_ASSET_LOADED(const_cast<char*>(m_assetName.c_str()));
}

bool LoopedParticle::Exists() const {
    return m_handle != -1 && GRAPHICS::DOES_PARTICLE_FX_LOOPED_EXIST(m_handle);
}

void LoopedParticle::Start(Entity entity, float scale, Vector3 offset, Vector3 rotation, int bone) {
    if (m_handle != -1) return;

    m_scale = scale;
    GRAPHICS::_SET_PTFX_ASSET_NEXT_CALL(const_cast<char*>(m_assetName.c_str()));
    
    if (bone == -1) {
        // MATCH C# LoopedParticle.cs: use (..., scale, 0, 0, 1) instead of (..., scale, false, false, false)
        // The last parameter (useRotation) is 1 (true) in C#, which might affect layer alignment.
        m_handle = GRAPHICS::START_PARTICLE_FX_LOOPED_ON_ENTITY(
            const_cast<char*>(m_fxName.c_str()), entity,
            offset.x, offset.y, offset.z,
            rotation.x, rotation.y, rotation.z,
            scale, false, false, true
        );
    }
    else {
        // Use the bone-specific native
        m_handle = GRAPHICS::_START_PARTICLE_FX_LOOPED_ON_ENTITY_BONE(
            const_cast<char*>(m_fxName.c_str()), entity,
            offset.x, offset.y, offset.z,
            rotation.x, rotation.y, rotation.z,
            bone, scale, false, false, false
        );
    }

    if (m_handle != -1 && m_handle != 0) {
        GRAPHICS::SET_PARTICLE_FX_LOOPED_ALPHA(m_handle, m_alpha);
    }
}

void LoopedParticle::Start(Vector3 position, float scale, Vector3 rotation) {
    if (m_handle != -1) return;

    m_scale = scale;
    GRAPHICS::_SET_PTFX_ASSET_NEXT_CALL(const_cast<char*>(m_assetName.c_str()));
    m_handle = GRAPHICS::START_PARTICLE_FX_LOOPED_AT_COORD(
        const_cast<char*>(m_fxName.c_str()), 
        position.x, position.y, position.z, 
        rotation.x, rotation.y, rotation.z, 
        scale, false, false, false, false
    );

    if (m_handle != -1 && m_handle != 0) {
        GRAPHICS::SET_PARTICLE_FX_LOOPED_ALPHA(m_handle, m_alpha);
    }
}

void LoopedParticle::Remove() {
    if (m_handle == -1) return;

    GRAPHICS::STOP_PARTICLE_FX_LOOPED(m_handle, false);
    GRAPHICS::REMOVE_PARTICLE_FX(m_handle, false);
    m_handle = -1;
}

void LoopedParticle::Remove(Vector3 position, float radius) {
    if (m_handle == -1) return;

    GRAPHICS::REMOVE_PARTICLE_FX_IN_RANGE(position.x, position.y, position.z, radius);
    m_handle = -1;
}

void LoopedParticle::Unload() {
    if (IsLoaded()) {
        STREAMING::_REMOVE_NAMED_PTFX_ASSET(const_cast<char*>(m_assetName.c_str()));
    }
}

void LoopedParticle::SetAlpha(float alpha) {
    m_alpha = alpha;
    if (Exists()) {
        GRAPHICS::SET_PARTICLE_FX_LOOPED_ALPHA(m_handle, alpha);
    }
}

void LoopedParticle::SetScale(float scale) {
    m_scale = scale;
    if (Exists()) {
        GRAPHICS::SET_PARTICLE_FX_LOOPED_SCALE(m_handle, scale);
    }
}

void LoopedParticle::SetColour(float r, float g, float b, float a) {
    if (Exists()) {
        GRAPHICS::SET_PARTICLE_FX_LOOPED_COLOUR(m_handle, r, g, b, false);
        if (a > 0.0f) {
            GRAPHICS::SET_PARTICLE_FX_LOOPED_ALPHA(m_handle, a);
        }
    }
}

void LoopedParticle::SetOffsets(Vector3 offset, Vector3 rotOffset) {
    if (Exists()) {
        GRAPHICS::SET_PARTICLE_FX_LOOPED_OFFSETS(m_handle, offset.x, offset.y, offset.z, rotOffset.x, rotOffset.y, rotOffset.z);
    }
}

void LoopedParticle::SetEvolution(const std::string& variableName, float value) {
    if (Exists()) {
        GRAPHICS::SET_PARTICLE_FX_LOOPED_EVOLUTION(m_handle, const_cast<char*>(variableName.c_str()), value, false);
    }
}
