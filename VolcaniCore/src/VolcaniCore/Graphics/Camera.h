#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <Core/Defines.h>
#include <Core/Template.h>

namespace VolcaniCore {

class Camera : public Derivable<Camera> {
public:
	enum class Type { Ortho, Stereo };

public:
	static Ref<Camera> Create(Camera::Type type);
	static Ref<Camera> Create(Camera::Type type, float fovOrRotation);

public:
	Camera(Camera::Type type);
	Camera(Camera::Type type, uint32_t width, uint32_t height,
			float near, float far);
	virtual ~Camera() = default;

	virtual void Resize(uint32_t width, uint32_t height);
	virtual void SetProjection(float near, float far);

	void SetPosition(const glm::vec3& pos);
	void SetDirection(const glm::vec3& dir);
	void SetPositionDirection(const glm::vec3& pos, const glm::vec3& dir);

	Camera::Type GetType() const { return m_Type; }

	const glm::vec3& GetPosition()  const { return Position; }
	const glm::vec3& GetDirection() const { return Direction; }
	
	uint32_t GetViewportWidth() const { return ViewportWidth; }
	uint32_t GetViewportHeight() const { return ViewportHeight; }
	float GetNear() const { return Near; }
	float GetFar()	const { return Far; }

	const glm::mat4& GetView()           const { return View; }
	const glm::mat4& GetProjection()     const { return Projection; }
	const glm::mat4& GetViewProjection() const { return ViewProjection; }

protected:
	glm::vec3 Position	= { 0.0f, 0.0f, 0.0f };
	glm::vec3 Direction = { 0.0f, 0.0f, -1.0f };

	uint32_t ViewportWidth  = 800;
	uint32_t ViewportHeight = 600;
	float Near = 0.001f;
	float Far  = 1000.0f;

	glm::mat4 Projection{ 1.0f };
	glm::mat4 View{ 1.0f };
	glm::mat4 ViewProjection{ 1.0f };

protected:
	virtual void CalculateView() = 0;
	virtual void CalculateProjection() = 0;

private:
	const Camera::Type m_Type;
};

}