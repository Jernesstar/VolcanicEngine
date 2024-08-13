#pragma once

#include <VolcaniCore/Renderer/Camera.h>

using namespace VolcaniCore;

namespace TheMazeIsLava {

class IsometricCamera : public Camera {
public:
	float R = 5.0f;

public:
	IsometricCamera()
		: Camera(Camera::Type::Ortho)
	{
		Position = R * glm::vec3{
			glm::sin(glm::radians(45.0f)),
			glm::sin(glm::radians(35.264f)),
			glm::sin(glm::radians(45.0f))
		};

		Direction = -glm::normalize(Position);

		CalculateProjection();
		CalculateView();
	}
	~IsometricCamera() = default;

private:
	void CalculateProjection() override {
		float asp = R * (ViewportWidth/ViewportHeight);
		Projection = glm::ortho(-asp, asp, -R, R, Near, Far);

		ViewProjection = Projection * View;
	}
	void CalculateView() override {
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		View = glm::lookAt(Position, Position + Direction, up);
		ViewProjection = Projection * View;
	}
};

}
