#include "StereographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.h"

namespace VolcaniCore {

StereographicCamera::StereographicCamera(float vertical_fov, float near_clip, float far_clip, uint32_t width, uint32_t height)
	: m_ViewportWidth(width), m_ViewportHeight(height)
{
	VOLCANICORE_ASSERT(m_ViewportWidth != 0 && m_ViewportHeight != 0, "Viewport width and height must not be 0");
	SetProjection(vertical_fov, near_clip, far_clip);
}

void StereographicCamera::SetProjection(float vertical_fov, float near_clip, float far_clip)
{
	VOLCANICORE_ASSERT(near_clip != 0, "Near clip must not be 0");

	m_VerticalFOV = vertical_fov;
	m_NearClip = near_clip;
	m_FarClip = far_clip;

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

void StereographicCamera::CalculateProjection() {
	Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
	InverseProjection = glm::inverse(Projection);
	ViewProjection = Projection * View;
}

void StereographicCamera::CalculateView() {
	View = glm::lookAt(Position, Position + ForwardDirection, glm::vec3(0, 1, 0));
	InverseView = glm::inverse(View);
	ViewProjection = Projection * View;
}

}