#include "CameraController.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Application.h"
#include "Core/Input.h"
#include "Events/EventSystem.h"

namespace VolcaniCore {

void CameraController::OnUpdate(TimeStep ts)
{
	glm::vec2 mouse_pos = Input::GetMousePosition();
	glm::vec2 delta = (mouse_pos - m_LastMousePosition) * 0.001f;
	m_LastMousePosition = mouse_pos;

	if(Input::MouseButtonPressed(Mouse::LeftButton) /*&& !Application::GetWindow()->IsImGuiFocused()*/)
		Input::SetCursorMode(CursorMode::Locked);
	else
	{
		Input::SetCursorMode(CursorMode::Normal);
		return;
	}

	glm::vec3 up_direction(0.0f, 1.0f, 0.0f);
	glm::vec3 forward_direction = m_Camera->GetDirection();
	glm::vec3 right_direction = glm::cross(forward_direction, up_direction);

	bool moved = false;
	glm::vec3 xyz;
	if(xyz.x = Input::KeyPressed(Controls[Control::Right]) - Input::KeyPressed(Controls[Control::Left]))
		m_Camera->Position += (float)xyz.x * right_direction * TranslationSpeed * (float)ts;
	if(xyz.y = Input::KeyPressed(Controls[Control::Up]) - Input::KeyPressed(Controls[Control::Down]))
		m_Camera->Position += (float)xyz.y * up_direction * TranslationSpeed * (float)ts;
	if(xyz.z = Input::KeyPressed(Controls[Control::Forward]) - Input::KeyPressed(Controls[Control::Backward]))
		m_Camera->Position += (float)xyz.z * forward_direction * TranslationSpeed * (float)ts;
	if(xyz.x || xyz.y || xyz.z)
		moved = true;

	if(delta.x != 0.0f || delta.y != 0.0f)
	{
		float pitch_delta = delta.y * RotationSpeed;
		float yaw_delta = delta.x * RotationSpeed;

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitch_delta, right_direction),
												glm::angleAxis(-yaw_delta, up_direction)));
		m_Camera->ForwardDirection = glm::rotate(q, forward_direction);
		moved = true;
	}

	if(moved)
		m_Camera->CalculateView();
}

void CameraController::OnMouseEvent(const MouseEvent& event)
{
}

}