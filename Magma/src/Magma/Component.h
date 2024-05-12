#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Core/Defines.h>
#include <Renderer/Texture.h>

using namespace VolcaniCore;

namespace Magma {

enum class ComponentType { EventListener, Tag, Texture, Transform, Unknown };

struct Component {
	const ComponentType Type;
	Component(ComponentType type) : Type(type) { }
};

struct EventListenerComponent : public Component {
	EventCallback<KeyPressedEvent>          OnKeyPressed;
	EventCallback<KeyReleasedEvent>         OnKeyReleased;
	EventCallback<MouseMovedEvent>          OnMouseMoved;
	EventCallback<MouseScrolledEvent>       OnMouseScrolled;
	EventCallback<MouseButtonPressedEvent>  OnMouseButtonPressed;
	EventCallback<MouseButtonReleasedEvent> OnMouseButtonReleased;
	EventCallback<WindowResizedEvent>       OnWindowResized;
	EventCallback<WindowClosedEvent>        OnWindowClosed;

	EventListenerComponent() : Component(ComponentType::EventListener) { }
	EventListenerComponent(const EventListenerComponent& other) = default;

	void RegisterCallbacks() {
		if(OnKeyPressed)          EventSystem::RegisterEventListener(OnKeyPressed);
		if(OnKeyReleased)         EventSystem::RegisterEventListener(OnKeyReleased);
		if(OnMouseMoved)          EventSystem::RegisterEventListener(OnMouseMoved);
		if(OnMouseScrolled)       EventSystem::RegisterEventListener(OnMouseScrolled);
		if(OnMouseButtonPressed)  EventSystem::RegisterEventListener(OnMouseButtonPressed);
		if(OnMouseButtonReleased) EventSystem::RegisterEventListener(OnMouseButtonReleased);
		if(OnWindowResized)       EventSystem::RegisterEventListener(OnWindowResized);
		if(OnWindowClosed)        EventSystem::RegisterEventListener(OnWindowClosed);
	}

	void UnregisterCallbacks() {
		if(OnKeyPressed)          EventSystem::UnregisterEventListener(OnKeyPressed);
		if(OnKeyReleased)         EventSystem::UnregisterEventListener(OnKeyReleased);
		if(OnMouseMoved)          EventSystem::UnregisterEventListener(OnMouseMoved);
		if(OnMouseScrolled)       EventSystem::UnregisterEventListener(OnMouseScrolled);
		if(OnMouseButtonPressed)  EventSystem::UnregisterEventListener(OnMouseButtonPressed);
		if(OnMouseButtonReleased) EventSystem::UnregisterEventListener(OnMouseButtonReleased);
		if(OnWindowResized)       EventSystem::UnregisterEventListener(OnWindowResized);
		if(OnWindowClosed)        EventSystem::UnregisterEventListener(OnWindowClosed);
	}
};

struct TagComponent : public Component {
	const std::string Tag;

	TagComponent() : Component(ComponentType::Tag), Tag("Null Tag") { }
	TagComponent(const std::string_view& tag) : Component(ComponentType::Tag), Tag(tag) { }
};

struct TextureComponent : public Component {
	Ptr<VolcaniCore::Texture> Texture;

	TextureComponent() : Component(ComponentType::Texture) { }
	TextureComponent(const std::string& path) : Component(ComponentType::Texture) {
		Texture = CreatePtr<VolcaniCore::Texture>(path);
	}
};

struct TransformComponent : public Component {
	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f};
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	TransformComponent() : Component(ComponentType::Transform) { }
	TransformComponent(glm::vec3 t, glm::vec3 r, glm::vec3 s)
		: Component(ComponentType::Transform), Translation(t), Rotation(r), Scale(s) { }

	glm::mat4 GetTransform() {
		return glm::translate(glm::mat4(1.0f), Translation)
			*  glm::toMat4(glm::quat(Rotation))
			*  glm::scale(glm::mat4(1.0f), Scale);
	}
};

}