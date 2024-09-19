#include "CameraController.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/Log.h"

namespace VolcaniCore {

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

	bool moved = false;

	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 forward = m_Camera->GetDirection();
	glm::vec3 right = glm::normalize(glm::cross(forward, up));

	float speed = TranslationSpeed * 0.001f;
	glm::ivec3 dir(0);
	glm::vec3 finalDir(0.0f);
	if(dir.x = Input::KeyPressed(m_Controls[Control::Right])
			 - Input::KeyPressed(m_Controls[Control::Left]))
	{
		finalDir += (float)dir.x * right;
	}
	if(dir.y = Input::KeyPressed(m_Controls[Control::Up])
			 - Input::KeyPressed(m_Controls[Control::Down]))
	{
		finalDir += (float)dir.y * up;
	}
	if(dir.z = Input::KeyPressed(m_Controls[Control::Forward])
			 - Input::KeyPressed(m_Controls[Control::Backward]))
	{
		finalDir += (float)dir.z * forward;
	}
	moved = dir.x || dir.y || dir.z;

	if(moved)
		m_Camera->Position += glm::normalize(finalDir) * speed * (float)ts;

	if((delta.x != 0.0f || delta.y != 0.0f) && RotationSpeed != 0.0f) {
		float pitchDelta = delta.y * RotationSpeed;
		float yawDelta = delta.x * RotationSpeed;

		glm::quat q = glm::cross(glm::angleAxis(-pitchDelta, right),
								 glm::angleAxis(-yawDelta, up));
		m_Camera->Direction = glm::rotate(glm::normalize(q), forward);
		moved = true;
	}

	if(moved)
		m_Camera->CalculateView();
}

}