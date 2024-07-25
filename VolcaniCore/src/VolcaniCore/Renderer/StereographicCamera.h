#pragma once

#include <cstdint>

#include "Camera.h"

namespace VolcaniCore {

class StereographicCamera : public Camera {
public:
	StereographicCamera();
	StereographicCamera(float verticalFOV);
	StereographicCamera(float verticalFOV,
						uint32_t width, uint32_t height,
						float near, float far, float rotation = 0.0f);
	~StereographicCamera() = default;

	void SetProjection(float verticalFOV, float near, float far);

	float GetVerticalFOV() const { return m_VerticalFOV; }

private:
	float m_VerticalFOV = 45.0f;

private:
	void CalculateView() override;
	void CalculateProjection() override;
};

}