#pragma once

#include "Camera.h"

namespace VolcaniCore {

class OrthographicCamera : public Camera {
public:
	OrthographicCamera() : Camera(CameraType::Ortho) { }
	OrthographicCamera(float width, float height,
					   float near, float far, float rotation = 0.0f);
	~OrthographicCamera() = default;

	void Resize(uint32_t width, uint32_t height) override {
		m_Width = width;
		m_Height = height;
		CalculateView();
	}

	void SetProjection(float width, float height, float near, float far);
	void SetRotation(float rotation) { m_Rotation = rotation; CalculateView(); }

	float GetRotation() const { return m_Rotation; }
	float GetWidth()	const { return m_Width; }
	float GetHeight()	const { return m_Height; }
	float GetNear()		const { return m_Near; }
	float GetFar()		const { return m_Far; }

private:
	float m_Rotation = 0.0f;
	float m_Width	= 0.0f, m_Height = 0.0f;
	float m_Near 	= 0.0f, m_Far	 = 0.0f;

private:
	void CalculateView() override;
	void CalculateProjection() override;
};

}