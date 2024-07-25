#include "StereographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.h"

namespace VolcaniCore {

StereographicCamera::StereographicCamera()
	: Camera(CameraType::Stereo) { }

StereographicCamera::StereographicCamera(float verticalFov)
	: Camera(CameraType::Stereo), m_VerticalFOV(verticalFov)
{
	CalculateProjection();
	CalculateView();
}

StereographicCamera::StereographicCamera(float verticalFov,
										 uint32_t width, uint32_t height,
										 float near, float far,
										 float rotation)
	: Camera(CameraType::Stereo, width, height, near, far, rotation),
		m_VerticalFOV(verticalFov)
{
	CalculateProjection();
	CalculateView();
}

void StereographicCamera::SetProjection(float verticalFov,
										float near, float far)
{
	VOLCANICORE_ASSERT(near != 0, "Near clip must not be 0");

	m_VerticalFOV = verticalFov;
	m_Near = near;
	m_Far = far;

	CalculateProjection();
}

void StereographicCamera::CalculateView() {
	glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	View = glm::lookAt(Position, Position + ForwardDirection, up);
	ViewProjection = Projection * View;
}

void StereographicCamera::CalculateProjection() {
	Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV),
									 (float)m_ViewportWidth,
									 (float)m_ViewportHeight,
									 m_NearClip, m_FarClip);
	ViewProjection = Projection * View;
}

}