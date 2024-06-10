#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.h"

namespace VolcaniCore {

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float rotation)
	: Camera(CameraType::Ortho), m_Rotation(rotation)
{
	SetProjection(left, right, bottom, top);
}

void OrthographicCamera::SetProjection(float left, float right, float bottom, float top, float near, float far) {
	m_Left	= left;		m_Right = right;
	m_Bottom = bottom;  m_Top = top;
	m_Near	= near,		m_Far = far;
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
	Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
	InverseProjection = glm::inverse(Projection);
	ViewProjection = Projection * View;
}

}