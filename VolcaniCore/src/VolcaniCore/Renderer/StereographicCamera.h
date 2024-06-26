#pragma once

#include <cstdint>

#include "Camera.h"

namespace VolcaniCore {

class StereographicCamera : public Camera {
public:
	StereographicCamera() : Camera(CameraType::Stereo) { }
	StereographicCamera(float verticalFOV, float nearClip, float farClip,
						uint32_t width, uint32_t height,
						float rotationSpeed = 0.3f);
	~StereographicCamera() = default;

	void Resize(uint32_t width, uint32_t height) override;
	void SetProjection(float verticalFOV, float nearClip, float farClip);

	float GetVerticalFOV() const { return m_VerticalFOV; }
	float GetNearClip() const { return m_NearClip; }
	float GetFarClip() const { return m_FarClip; }
	float GetRotationSpeed() const { return m_RotationSpeed; }
	uint32_t GetViewportWidth() const { return m_ViewportWidth; }
	uint32_t GetViewportHeight() const { return m_ViewportHeight; }

private:
	float m_VerticalFOV = 45.0f;
	float m_NearClip = 0.1f;
	float m_FarClip = 100.0f;
	float m_RotationSpeed = 0.3f;
	uint32_t m_ViewportWidth = 1, m_ViewportHeight = 1;

private:
	void CalculateView() override;
	void CalculateProjection() override;
};

}