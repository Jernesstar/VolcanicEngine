#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <Core/Defines.h>
#include <Core/Assert.h>

namespace VolcaniCore {

class Camera {
public:
	enum class Type { Ortho, Stereo };

public:
	static Ref<Camera> Create(Camera::Type type);
	static Ref<Camera> Create(Camera::Type type, float fovOrRotation);

public:
	Camera(Camera::Type type)
		: m_Type(type) { }

	Camera(Camera::Type type,
			uint32_t width, uint32_t height, float near, float far)
		: m_Type(type), ViewportWidth(width), ViewportHeight(height),
			Near(near), Far(far)
	{
		VOLCANICORE_ASSERT(width != 0 && height != 0,
							"Viewport width and height must not be 0");
		VOLCANICORE_ASSERT(near != 0 && far != 0,
							"Near and far clip must not be non-zero");
	}
	virtual ~Camera() = default;

	void Resize(uint32_t width, uint32_t height) {
		if(width == ViewportWidth && height == ViewportHeight) {
			VOLCANICORE_LOG_WARNING("Viewport is already this size!");
			return;
		}
		if(width == 0 || height == 0) {
			VOLCANICORE_LOG_WARNING("Viewport width and height may not be 0");
			return;
		}

		ViewportWidth = width;
		ViewportHeight = height;
		CalculateProjection();
	}

	void SetProjection(float near, float far) {
		if(near == Near && far == Far) {
			VOLCANICORE_LOG_WARNING(
				"[SetProjection]: Projection is already configured as such!");
			return;
		}
		if(near == 0 || far == 0) {
			VOLCANICORE_LOG_WARNING(
				"[SetProjection]: Near and far clips may not be 0");
			return;
		}
	
		Near = near;
		Far = far;
		CalculateProjection();
	}

	void SetPosition(const glm::vec3& position) {
		Position = position;
		CalculateView();
	}
	void SetDirection(const glm::vec3& direction) {
		Direction = direction;
		CalculateView();
	}
	void SetPositionDirection(const glm::vec3& position,
							  const glm::vec3& direction)
	{
		Position = position;
		Direction = direction;
		CalculateView();
	}

	Camera::Type GetType() const { return m_Type; }

	const glm::vec3& GetPosition()  const { return Position; }
	const glm::vec3& GetDirection() const { return Direction; }

	const glm::mat4& GetView()           const { return View; }
	const glm::mat4& GetProjection()     const { return Projection; }
	const glm::mat4& GetViewProjection() const { return ViewProjection; }

	uint32_t GetViewportWidth() const { return ViewportWidth; }
	uint32_t GetViewportHeight() const { return ViewportHeight; }
	float GetNear() const { return Near; }
	float GetFar()	const { return Far; }

	template<typename TDerived>
	requires std::derived_from<TDerived, Camera>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	glm::vec3 Position	= { 0.0f, 0.0f, 0.0f };
	glm::vec3 Direction = { 0.0f, 0.0f, -1.0f };

	float Near = 0.001f;
	float Far  = 1000.0f;
	uint32_t ViewportWidth  = 800;
	uint32_t ViewportHeight = 600;

	glm::mat4 Projection{ 1.0f };
	glm::mat4 InverseProjection{ 1.0f };
	glm::mat4 View{ 1.0f };
	glm::mat4 InverseView{ 1.0f };
	glm::mat4 ViewProjection{ 1.0f };

protected:
	virtual void CalculateView() = 0;
	virtual void CalculateProjection() = 0;

private:
	const Camera::Type m_Type;
};

}