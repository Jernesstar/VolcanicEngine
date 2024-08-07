#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.h"

namespace VolcaniCore {

OrthographicCamera::OrthographicCamera()
	: Camera(Camera::Type::Ortho)
{
	CalculateProjection();
	CalculateView();
}

OrthographicCamera::OrthographicCamera(float rotation)
	: Camera(Camera::Type::Ortho), m_Rotation(rotation)
{
	CalculateProjection();
	CalculateView();
}

OrthographicCamera::OrthographicCamera(float width, float height,
									   float near, float far, float rotation)
	: Camera(Camera::Type::Ortho, width, height, near, far),
		m_Rotation(rotation)
{
	CalculateProjection();
	CalculateView();
}

void OrthographicCamera::SetRotation(float rotation) {
	m_Rotation = rotation;
	CalculateView();
}

void OrthographicCamera::CalculateView() {
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position)
						* glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation),
									  glm::vec3{ 0, 0, 1 });

	View = glm::inverse(transform);
	ViewProjection = Projection * View;
}

void OrthographicCamera::CalculateProjection() {
	Projection = glm::ortho(-m_ViewportWidth  / 2.0f, m_ViewportWidth  / 2.0f,
							-m_ViewportHeight / 2.0f, m_ViewportHeight / 2.0f,
							m_Near, m_Far);
	ViewProjection = Projection * View;
}

}