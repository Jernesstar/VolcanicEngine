#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace VolcaniCore {

enum class CameraType { Ortho, Stereo };

class Camera {
public:
	const CameraType Type;

	Camera(CameraType type)
		: Type(type)
	{
		CalculateProjection();
		CalculateView();
	}
	Camera(CameraType type,
		   uint32_t width, uint32_t height,
		   float near, float far, float rotation)
		: Type(type), m_ViewportWidth(width), m_ViewportHeight(height),
			m_Near(near), m_Far(far), m_Rotation(rotation)
	{
		VOLCANICORE_ASSERT(width != 0 && height != 0,
						   "Viewport width and height must not be 0");
		VOLCANICORE_ASSERT(near != 0 && far != 0,
						   "Near and far clip must not be non-zero");
	}
	virtual ~Camera() = default;

	void Resize(uint32_t width, uint32_t height) {
		if(width == m_ViewportWidth && height == m_ViewportHeight)
			return;

		VOLCANICORE_ASSERT(width != 0 && height != 0,
						   "Viewport width and height must be non-zero");

		m_ViewportWidth = width;
		m_ViewportHeight = height;
		CalculateProjection();
	}

	void SetPosition(const glm::vec3& position) {
		Position = position;
		CalculateView();
	}
	void SetDirection(const glm::vec3& direction) {
		ForwardDirection = direction;
		CalculateView();
	}
	void SetPositionDirection(const glm::vec3& position,
							  const glm::vec3& direction)
	{
		Position = position;
		ForwardDirection = direction;
		CalculateView();
	}
	void SetRotation(float rotation) {
		m_Rotation = rotation;
		CalculateView();
	}

	const glm::vec3& GetPosition() const { return Position; }
	const glm::vec3& GetDirection() const { return ForwardDirection; }

	const glm::mat4& GetView()           const { return View; }
	const glm::mat4& GetProjection()     const { return Projection; }
	const glm::mat4& GetViewProjection() const { return ViewProjection; }

	float GetViewportWidth()  const { return m_ViewportWidth; }
	float GetViewportHeight() const { return m_ViewportHeight; }
	float GetNear() const { return m_Near; }
	float GetFar()	const { return m_Far; }
	float GetRotation() const { return m_Rotation; }

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 ForwardDirection = { 0.0f, 0.0f, -1.0f };

	float m_NearClip = 0.1f;
	float m_FarClip	 = 100.0f;
	float m_Rotation = 0.0f;
	uint32_t m_ViewportWidth  = 800,
	uint32_t m_ViewportHeight = 600;

	glm::mat4 Projection{ 1.0f };
	glm::mat4 InverseProjection{ 1.0f };
	glm::mat4 View{ 1.0f };
	glm::mat4 InverseView{ 1.0f };
	glm::mat4 ViewProjection{ 1.0f };

	virtual void CalculateView() = 0;
	virtual void CalculateProjection() = 0;

	friend class CameraController;
};

}