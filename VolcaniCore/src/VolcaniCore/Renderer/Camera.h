#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <Core/Assert.h>

namespace VolcaniCore {

class Camera {
public:
	enum class Type { Ortho, Stereo };

public:
	Camera(Camera::Type type)
		: m_Type(type) { }

	Camera(Camera::Type type,
			uint32_t width, uint32_t height, float near, float far)
		: m_Type(type), m_ViewportWidth(width), m_ViewportHeight(height),
			m_Near(near), m_Far(far)
	{
		VOLCANICORE_ASSERT(width != 0 && height != 0,
							"Viewport width and height must not be 0");
		VOLCANICORE_ASSERT(near != 0 && far != 0,
							"Near and far clip must not be non-zero");
	}
	virtual ~Camera() = default;

	Camera::Type GetType() { return m_Type; }

	void Resize(uint32_t width, uint32_t height) {
		if(width == m_ViewportWidth && height == m_ViewportHeight) {
			VOLCANICORE_LOG_WARNING("Viewport is already this size!");
			return;
		}
		if(width == 0 || height == 0) {
			VOLCANICORE_LOG_WARNING("Viewport width and height may not be 0");
			return;
		}

		m_ViewportWidth = width;
		m_ViewportHeight = height;
		CalculateProjection();
	}

	void SetProjection(float near, float far) {
		if(near == m_Near && far == m_Far) {
			VOLCANICORE_LOG_WARNING("Projection is already this size!");
			return;
		}
		if(near == 0 || far == 0) {
			VOLCANICORE_LOG_WARNING("Near and far clips may not be 0");
			return;
		}
	
		m_Near = near;
		m_Far = far;
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

	const glm::vec3& GetPosition() const { return Position; }
	const glm::vec3& GetDirection() const { return ForwardDirection; }

	const glm::mat4& GetView()           const { return View; }
	const glm::mat4& GetProjection()     const { return Projection; }
	const glm::mat4& GetViewProjection() const { return ViewProjection; }

	float GetViewportWidth() const { return m_ViewportWidth; }
	float GetViewportHeight() const { return m_ViewportHeight; }
	float GetNear() const { return m_Near; }
	float GetFar()	const { return m_Far; }

	template<typename TDerived>
	requires std::derived_from<TDerived, Camera>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	const Camera::Type m_Type;

	// TODO: Consistent naming conventions?
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 ForwardDirection = { 0.0f, 0.0f, -1.0f };

	float m_Near = 0.1f;
	float m_Far	 = 100.0f;
	uint32_t m_ViewportWidth  = 800;
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