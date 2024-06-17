#pragma once

#include <memory>

#include "Camera.h"

#include "Events/Events.h"
#include "Core/Time.h"
#include "Core/Codes.h"

namespace VolcaniCore {

enum class Control { Up, Down, Left, Right, Forward, Backward };

struct MovementControls {
	std::unordered_map<Control, Key> Map;

	MovementControls(std::unordered_map<Control, Key> map = { })
		: Map(GetControls(map)) { }

	Key operator [](const Control& control) const { return Map.at(control); }

private:
	std::unordered_map<Control, Key> GetControls(std::unordered_map<Control, Key> map) {
		std::unordered_map<Control, Key> controls;
		controls[Control::Up]       = Get(map, Control::Up,       Key::Q);
		controls[Control::Down]     = Get(map, Control::Down,     Key::E);
		controls[Control::Left]     = Get(map, Control::Left,     Key::A);
		controls[Control::Right]    = Get(map, Control::Right,    Key::D);
		controls[Control::Forward]  = Get(map, Control::Forward,  Key::W);
		controls[Control::Backward] = Get(map, Control::Backward, Key::S);
		return controls;
	}

	Key Get(std::unordered_map<Control, Key> map, Control control, Key default_val) {
		return map.find(control) != map.end() ? map[control] : default_val;
	}
};

class CameraController {
public:
	float TranslationSpeed = 0.005f;
	float RotationSpeed = 0.6f;
	const MovementControls Controls;

public:
	CameraController(Ref<Camera> camera, const MovementControls& controls = { })
		: m_Camera(camera), Controls(controls) { }

	void OnUpdate(TimeStep ts);
	void OnResize(uint32_t width, uint32_t height);

private:
	glm::vec2 m_LastMousePosition = { 0.0f, 0.0f };
	Ref<Camera> m_Camera;
};

}
