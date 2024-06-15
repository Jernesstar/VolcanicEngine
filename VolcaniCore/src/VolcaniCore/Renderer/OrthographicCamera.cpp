#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.h"

namespace VolcaniCore {

OrthographicCamera::OrthographicCamera(float width, float height, float near, float far, float rotation)
	: Camera(CameraType::Ortho), m_Rotation(rotation)
{
	SetProjection(width, height, near, far);
}

void OrthographicCamera::SetProjection(float width, float height, float near, float far) {
	m_Width	= width; m_Height = height;
	m_Near	= near,	 m_Far = far;
	CalculateProjection();
}

void OrthographicCamera::CalculateView() {
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position)
						* glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3{ 0, 0, 1 });

	View = glm::inverse(transform);
	InverseView = glm::inverse(View);
	ViewProjection = Projection * View;
}

void OrthographicCamera::CalculateProjection() {
	Projection = glm::ortho(-m_Width/2.0f, m_Width/2.0f, -m_Height/2.0f, m_Height/2.0f, m_Near, m_Far);
	InverseProjection = glm::inverse(Projection);
	ViewProjection = Projection * View;
}

}