#include "StereographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.h"

namespace VolcaniCore {

StereographicCamera::StereographicCamera()
	: Camera(Camera::Type::Stereo)
{
	CalculateProjection();
	CalculateView();
}

StereographicCamera::StereographicCamera(float verticalFov)
	: Camera(Camera::Type::Stereo), m_VerticalFOV(verticalFov)
{
	CalculateProjection();
	CalculateView();
}

StereographicCamera::StereographicCamera(float verticalFov,
										 uint32_t width, uint32_t height,
										 float near, float far)
	: Camera(Camera::Type::Stereo, width, height, near, far),
		m_VerticalFOV(verticalFov)
{
	CalculateProjection();
	CalculateView();
}

void StereographicCamera::SetVerticalFOV(float verticalFov) {
	m_VerticalFOV = verticalFov;
	CalculateProjection();
}

void StereographicCamera::CalculateView() {
	glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	View = glm::lookAt(Position, Position + Direction, up);
	ViewProjection = Projection * View;
}

void StereographicCamera::CalculateProjection() {
	Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV),
									 (float)m_ViewportWidth,
									 (float)m_ViewportHeight, Near, Far);
	ViewProjection = Projection * View;
}

}