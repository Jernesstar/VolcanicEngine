#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace VolcaniCore {

enum class CameraType { Ortho, Stereo };

class Camera {
public:
	Camera(CameraType type) : Type(type) { }
	~Camera() = default;

	CameraType GetType() const { return Type; }

	const glm::vec3& GetPosition()       const { return Position; }
	const glm::vec3& GetDirection()      const { return ForwardDirection; }

	const glm::mat4& GetProjection()     const { return Projection; }
	const glm::mat4& GetView()           const { return View; }
	const glm::mat4& GetViewProjection() const { return ViewProjection; }

	void SetPosition(const glm::vec3& position) { Position = position; CalculateView(); }
	void SetDirection(const glm::vec3& direction) { ForwardDirection = direction; CalculateView(); }

protected:
	CameraType Type;

	glm::mat4 Projection{ 1.0f };
	glm::mat4 InverseProjection{ 1.0f };
	glm::mat4 View{ 1.0f };
	glm::mat4 InverseView{ 1.0f };
	glm::mat4 ViewProjection{ 1.0f };

	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 ForwardDirection = { 0.0f, 0.0f, -1.0f };

	virtual void CalculateView() = 0;
	virtual void CalculateProjection() = 0;

	friend class CameraController;
	// friend class ::Magma::SceneSerializer;
};

}