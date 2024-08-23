#include "CameraController.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Application.h"
#include "Core/Input.h"

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
	glm::vec3 right = glm::cross(forward, up);

	float speed = TranslationSpeed * 0.001f;
	glm::ivec3 dir;
	if(dir.x = Input::KeyPressed(m_Controls[Control::Right])
			 - Input::KeyPressed(m_Controls[Control::Left]))
	{
		m_Camera->Position += (float)dir.x * right * speed * (float)ts;
	}
	if(dir.y = Input::KeyPressed(m_Controls[Control::Up])
			 - Input::KeyPressed(m_Controls[Control::Down]))
	{
		m_Camera->Position += (float)dir.y * up * speed * (float)ts;
	}
	if(dir.z = Input::KeyPressed(m_Controls[Control::Forward])
			 - Input::KeyPressed(m_Controls[Control::Backward]))
	{
		m_Camera->Position += (float)dir.z * forward * speed * (float)ts;
	}
	moved = dir.x || dir.y || dir.z;

	if((delta.x != 0.0f || delta.y != 0.0f) & RotationSpeed != 0.0f) {
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