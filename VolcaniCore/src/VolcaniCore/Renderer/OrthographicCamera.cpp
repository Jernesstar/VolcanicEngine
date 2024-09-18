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

OrthographicCamera::OrthographicCamera(uint32_t width, uint32_t height,
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
	// lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	View = glm::inverse(transform);
	ViewProjection = Projection * View;
}

void OrthographicCamera::CalculateProjection() {
	float asp = ViewportWidth/ViewportHeight;
	Projection = glm::ortho(-asp, asp, -1.0f/asp, 1.0f/asp, Near, Far);
	ViewProjection = Projection * View;
}

}