#include "StereographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.h"

namespace VolcaniCore {

StereographicCamera::StereographicCamera(float verticalFov, float nearClip, float farClip, uint32_t width, uint32_t height, float rotationSpeed)
	: Camera(CameraType::Stereo), m_ViewportWidth(width), m_ViewportHeight(height), m_RotationSpeed(rotationSpeed)
{
	VOLCANICORE_ASSERT(m_ViewportWidth != 0 && m_ViewportHeight != 0, "Viewport width and height must not be 0");
	SetProjection(verticalFov, nearClip, farClip);
}

void StereographicCamera::SetProjection(float verticalFov, float nearClip, float farClip) {
	VOLCANICORE_ASSERT(nearClip != 0, "Near clip must not be 0");

	m_VerticalFOV = verticalFov;
	m_NearClip = nearClip;
	m_FarClip = farClip;

	CalculateProjection();
}

void StereographicCamera::Resize(uint32_t width, uint32_t height) {
	if(width == m_ViewportWidth && height == m_ViewportHeight)
		return;

	VOLCANICORE_ASSERT(width != 0 && height != 0, "Viewport width and height must not be 0");

	m_ViewportWidth = width;
	m_ViewportHeight = height;
	CalculateProjection();
}

void StereographicCamera::CalculateView() {
	View = glm::lookAt(Position, Position + ForwardDirection, glm::vec3(0, 1, 0));
	InverseView = glm::inverse(View);
	ViewProjection = Projection * View;
}

void StereographicCamera::CalculateProjection() {
	Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
	InverseProjection = glm::inverse(Projection);
	ViewProjection = Projection * View;
}

}