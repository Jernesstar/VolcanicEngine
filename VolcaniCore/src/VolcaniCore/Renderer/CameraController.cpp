#include "CameraController.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Application.h"
#include "Core/Input.h"
#include "Event/Events.h"

namespace VolcaniCore {

void CameraController::OnUpdate(TimeStep ts)
{
	glm::vec2 mousePosition = Input::GetMousePosition();
	glm::vec2 delta = (mousePosition - m_LastMousePosition) * 0.001f;
	m_LastMousePosition = mousePosition;

	if(Input::MouseButtonPressed(Mouse::LeftButton))
		Input::SetCursorMode(CursorMode::Locked);
	else {
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	glm::vec3 upDir(0.0f, 1.0f, 0.0f);
	glm::vec3 forwardDir = m_Camera->GetDirection();
	glm::vec3 rightDir = glm::cross(forwardDir, upDir);

	bool moved = false;
	glm::ivec3 xyz;
	if(xyz.x = Input::KeyPressed(Controls[Control::Right])
			 - Input::KeyPressed(Controls[Control::Left]))
	{
		m_Camera->Position += xyz.x * rightDir * TranslationSpeed * (float)ts;
	}
	if(xyz.y = Input::KeyPressed(Controls[Control::Up])
			 - Input::KeyPressed(Controls[Control::Down]))
	{
		m_Camera->Position += xyz.y * upDir * TranslationSpeed * (float)ts;
	}
	if(xyz.z = Input::KeyPressed(Controls[Control::Forward])
			 - Input::KeyPressed(Controls[Control::Backward]))
	{
		m_Camera->Position += xyz.z * forwardDir * TranslationSpeed * (float)ts;
	}
	if(xyz.x || xyz.y || xyz.z)
		moved = true;

	if(delta.x != 0.0f || delta.y != 0.0f) {
		float pitchDelta = delta.y * RotationSpeed;
		float yawDelta = delta.x * RotationSpeed;

		glm::quat q = glm::cross(glm::angleAxis(-pitchDelta, rightDir),
								 glm::angleAxis(-yawDelta, upDir));
		m_Camera->Direction = glm::rotate(glm::normalize(q), forwardDir);
		moved = true;
	}

	if(moved)
		m_Camera->CalculateView();
}

void CameraController::OnResize(uint32_t width, uint32_t height) {
	m_Camera->Resize(width, height);
}

}