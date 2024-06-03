#pragma once

#include <VolcaniCore/Core/Defines.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <VolcaniCore/Events/EventCallback.h>
#include <VolcaniCore/Events/EventSystem.h>
#include <VolcaniCore/Renderer/Texture.h>

using namespace VolcaniCore;

namespace Magma {

enum class ComponentType { EventListener, Tag, Texture, Transform, Unknown };

struct Component {
	const ComponentType Type;
	Component(ComponentType type) : Type(type) { }
	Component(const Component& other) = default;
	virtual ~Component() = default;
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
		if(OnKeyPressed)          EventSystem::RegisterListener(OnKeyPressed);
		if(OnKeyReleased)         EventSystem::RegisterListener(OnKeyReleased);
		if(OnMouseMoved)          EventSystem::RegisterListener(OnMouseMoved);
		if(OnMouseScrolled)       EventSystem::RegisterListener(OnMouseScrolled);
		if(OnMouseButtonPressed)  EventSystem::RegisterListener(OnMouseButtonPressed);
		if(OnMouseButtonReleased) EventSystem::RegisterListener(OnMouseButtonReleased);
		if(OnWindowResized)       EventSystem::RegisterListener(OnWindowResized);
		if(OnWindowClosed)        EventSystem::RegisterListener(OnWindowClosed);
	}

	void UnregisterCallbacks() {
		if(OnKeyPressed)          EventSystem::UnregisterListener(OnKeyPressed);
		if(OnKeyReleased)         EventSystem::UnregisterListener(OnKeyReleased);
		if(OnMouseMoved)          EventSystem::UnregisterListener(OnMouseMoved);
		if(OnMouseScrolled)       EventSystem::UnregisterListener(OnMouseScrolled);
		if(OnMouseButtonPressed)  EventSystem::UnregisterListener(OnMouseButtonPressed);
		if(OnMouseButtonReleased) EventSystem::UnregisterListener(OnMouseButtonReleased);
		if(OnWindowResized)       EventSystem::UnregisterListener(OnWindowResized);
		if(OnWindowClosed)        EventSystem::UnregisterListener(OnWindowClosed);
	}
};

struct TagComponent : public Component {
	std::string Tag;

	TagComponent() : Component(ComponentType::Tag), Tag("Null Tag") { }
	TagComponent(const std::string_view& tag) : Component(ComponentType::Tag), Tag(tag) { }
	TagComponent(const TagComponent& other) = default;
};

struct TextureComponent : public Component {
	Ref<VolcaniCore::Texture> Texture;

	TextureComponent() : Component(ComponentType::Texture) { }
	TextureComponent(const std::string& path)
		: Component(ComponentType::Texture) { Texture = CreateRef<VolcaniCore::Texture>(path); }
	TextureComponent(const TextureComponent& other) = default;
};

struct TransformComponent : public Component {
	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f};
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	TransformComponent() : Component(ComponentType::Transform) { }
	TransformComponent(glm::vec3 t, glm::vec3 r, glm::vec3 s)
		: Component(ComponentType::Transform), Translation(t), Rotation(r), Scale(s) { }
	TransformComponent(const TransformComponent& other) = default;

	glm::mat4 GetTransform() {
		return glm::translate(glm::mat4(1.0f), Translation)
			*  glm::toMat4(glm::quat(Rotation))
			*  glm::scale(glm::mat4(1.0f), Scale);
	}
};

}