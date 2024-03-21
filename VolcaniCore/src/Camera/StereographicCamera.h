#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Camera.h"

namespace VolcaniCore {

class StereographicCamera : public Camera {
public:
    StereographicCamera() = default;
    StereographicCamera(float verticalFOV, float nearClip, float farClip, uint32_t width, uint32_t height);
    ~StereographicCamera() = default;

    void SetProjection(float verticalFOV, float nearClip, float farClip);
    void Resize(uint32_t width, uint32_t height);

private:
    void CalculateView() override;
    void CalculateProjection() override;

private:
    float m_VerticalFOV = 45.0f;
    float m_NearClip = 0.1f;
    float m_FarClip = 100.0f;
    float m_RotationSpeed = 0.3f;

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};

}