#include "CameraController.h"

#define GLM_ENABLE_EXPERIMENTAL

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

CameraController::~CameraController() {
	Events::UnregisterListener<WindowResizedEvent>(m_CallbackID);
}

void CameraController::SetCamera(Ref<Camera> camera) {
	m_Camera = camera;

	Events::UnregisterListener<WindowResizedEvent>(m_CallbackID);
	m_CallbackID = Events::RegisterListener<WindowResizedEvent>(
		[cam = m_Camera](const WindowResizedEvent& event)
		{
			cam->Resize(event.Width, event.Height);
		});
}

void CameraController::OnUpdate(TimeStep ts) {
	glm::vec2 mousePosition = Input::GetMousePosition();
	glm::vec2 delta = (mousePosition - m_LastMousePosition) * 0.001f;
	m_LastMousePosition = mousePosition;

	if(!Input::MouseButtonPressed(Mouse::LeftButton)) {
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	Input::SetCursorMode(CursorMode::Locked);

	glm::vec3 finalPos = m_Camera->GetPosition();
	glm::vec3 finalDir;
	bool moved = false;

	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 forward = m_Camera->GetDirection();
	glm::vec3 right = glm::normalize(glm::cross(forward, up));

	glm::ivec3 inputDir(0);
	inputDir.x = Input::KeyPressed(m_Controls[Control::Right])
			   - Input::KeyPressed(m_Controls[Control::Left]);
	inputDir.y = Input::KeyPressed(m_Controls[Control::Up])
			   - Input::KeyPressed(m_Controls[Control::Down]);
	inputDir.z = Input::KeyPressed(m_Controls[Control::Forward])
			   - Input::KeyPressed(m_Controls[Control::Backward]);

	if(moved = (inputDir.x || inputDir.y || inputDir.z)) {
		glm::vec3 moveDir(0.0f);
		moveDir += (float)inputDir.x * right;
		moveDir += (float)inputDir.y * up;
		moveDir += (float)inputDir.z * forward;
		float moveSpeed = TranslationSpeed * 0.001f;
		finalPos += glm::normalize(moveDir) * moveSpeed * (float)ts;
	}

	if(moved |= (delta != glm::vec2(0.0f) && RotationSpeed != 0.0f)) {
		float pitchDelta = delta.y * RotationSpeed;
		float yawDelta   = delta.x * RotationSpeed;

		glm::quat q = glm::cross(glm::angleAxis(-pitchDelta, right),
								 glm::angleAxis(-yawDelta, up));
		finalDir = glm::rotate(glm::normalize(q), forward);
	}

	if(moved)
		m_Camera->SetPositionDirection(finalPos, finalDir);
}

}