#include "CameraController.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/Log.h"

namespace VolcaniCore {

CameraController::CameraController(const MovementControls& controls)
	: m_Controls(controls) { }

CameraController::CameraController(Ref<Camera> camera) {
	SetCamera(camera);
}

void CameraController::SetCamera(Ref<Camera> camera) {
	m_Camera = camera;

	Events::RegisterListener<WindowResizedEvent>(
		[cam = m_Camera](const WindowResizedEvent& event)
		{
			cam->Resize(event.Width, event.Height);
		});
}

void CameraController::OnUpdate(TimeStep ts) {
	if(!m_Camera)
		return;

	glm::vec2 mousePosition = Input::GetMousePosition();
	glm::vec2 delta = (mousePosition - m_LastMousePosition) * 0.001f;
	m_LastMousePosition = mousePosition;

	if(Input::MouseButtonPressed(Mouse::LeftButton))
		Input::SetCursorMode(CursorMode::Locked);
	else {
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 forward = m_Camera->GetDirection();
	glm::vec3 right = glm::normalize(glm::cross(forward, up));

	glm::ivec3 dir(0);
	dir.x = Input::KeyPressed(m_Controls[Control::Right])
		  - Input::KeyPressed(m_Controls[Control::Left]);
	dir.y = Input::KeyPressed(m_Controls[Control::Up])
		  - Input::KeyPressed(m_Controls[Control::Down]);
	dir.z = Input::KeyPressed(m_Controls[Control::Forward])
		  - Input::KeyPressed(m_Controls[Control::Backward]);

	glm::vec3 finalDir(0.0f);
	finalDir += (float)dir.x * right;
	finalDir += (float)dir.y * up;
	finalDir += (float)dir.z * forward;
	float speed = TranslationSpeed * 0.001f;

	bool moved = false;
	if(moved = dir.x || dir.y || dir.z)
		m_Camera->Position += glm::normalize(finalDir) * speed * (float)ts;

	if(moved |= (delta.x != 0.0f || delta.y != 0.0f) && RotationSpeed != 0.0f) {
		float pitchDelta = delta.y * RotationSpeed;
		float yawDelta   = delta.x * RotationSpeed;

		glm::quat q = glm::cross(glm::angleAxis(-pitchDelta, right),
								 glm::angleAxis(-yawDelta, up));
		m_Camera->Direction = glm::rotate(glm::normalize(q), forward);
	}

	if(moved)
		m_Camera->CalculateView();
}

}