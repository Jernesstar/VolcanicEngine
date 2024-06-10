#pragma once

#include "Camera.h"

namespace VolcaniCore {

class OrthographicCamera : public Camera {
public:
	OrthographicCamera() : Camera(CameraType::Ortho) { }
	OrthographicCamera(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f, float rotation = 0.0f);
	~OrthographicCamera() = default;

	void Resize(uint32_t width, uint32_t height) override {
		m_Right = width;
		m_Top = height;
		CalculateView();
	}

	void SetProjection(float left, float right, float bottom, float top);
	void SetRotation(float rotation) { m_Rotation = rotation; CalculateView(); }

	float GetRotation() const { return m_Rotation; }
	float GetLeft()		const { return m_Left; }
	float GetRight()	const { return m_Right; }
	float GetBottom()	const { return m_Bottom; }
	float GetTop()		const { return m_Top; }

private:
	float m_Rotation = 0.0f;
	float m_Left	= 0.0f, m_Right	= 0.0f;
	float m_Bottom	= 0.0f, m_Top 	= 0.0f;
	float m_Near 	= 0.0f, m_Far	= 0.0f;

private:
	void CalculateView() override;
	void CalculateProjection() override;
};

}