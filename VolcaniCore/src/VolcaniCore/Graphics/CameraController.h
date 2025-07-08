#pragma once

#include "Camera.h"

#include "Core/Time.h"
#include "Core/Codes.h"
#include "Event/Events.h"

namespace VolcaniCore {

enum class Control { Up, Down, Left, Right, Forward, Backward };

using ControlMap = std::unordered_map<Control, Key>;

class MovementControls {
public:
	MovementControls(const ControlMap& map = { })
		: m_Map(GetControls(map)) { }

	Key operator [](Control control) const { return m_Map.at(control); }

private:
	ControlMap m_Map;

	ControlMap GetControls(const ControlMap& map) const {
		ControlMap controls;
		controls[Control::Up]		= Get(map, Control::Up,		  Key::Q);
		controls[Control::Down]		= Get(map, Control::Down,	  Key::E);
		controls[Control::Left]		= Get(map, Control::Left,	  Key::A);
		controls[Control::Right]	= Get(map, Control::Right,	  Key::D);
		controls[Control::Forward]	= Get(map, Control::Forward,  Key::W);
		controls[Control::Backward] = Get(map, Control::Backward, Key::S);
		return controls;
	}

	Key Get(const ControlMap& map, Control control, Key defaultVal) const {
		return map.find(control) != map.end() ? map.at(control) : defaultVal;
	}
};

class CameraController {
public:
	float TranslationSpeed = 2.0f;
	float RotationSpeed = 1.0f;

public:
	CameraController(const MovementControls& controls = { });
	CameraController(Ref<Camera> camera);
	~CameraController() = default;

	void OnUpdate(TimeStep ts);

	void SetControls(const MovementControls& controls) {
		m_Controls = controls;
	}

	Ref<Camera> GetCamera() const { return m_Camera; }
	void SetCamera(Ref<Camera> camera);

private:
	Ref<Camera> m_Camera;
	MovementControls m_Controls;

	glm::vec2 m_LastMousePosition = { 0.0f, 0.0f };
};

}
