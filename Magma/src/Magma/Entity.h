#pragma once

#include <unordered_map>

#include "Component.h"

namespace Magma {

class Entity {
public:
	Entity() = default;
	~Entity() = default;
	
	template<typename TComponent>
	bool Has() { return m_Components.count(TypeOf<TComponent>::Get()) == 1; }

	template<typename TComponent, typename... Args>
	TComponent& Add(Args&& ...args) {
		if(Has<TComponent>()) return Get<TComponent>();
		m_Components[TypeOf<TComponent>::Get()] = CreatePtr<TComponent>(std::forward<Args>(args)...);
		return *((TComponent*)m_Components[TypeOf<TComponent>::Get()].get());
	}

	template<typename TComponent>
	TComponent& Get() {
		if(!Has<TComponent>()) return Add<TComponent>();
		return *((TComponent*)m_Components[TypeOf<TComponent>::Get()].get());
	}

	template<typename TComponent>
	void Remove() {
		m_Components.erase(TypeOf<TComponent>::Get());
	}

private:
	template<typename TComponent>
	struct TypeOf { static ComponentType Get() { return ComponentType::Unknown; } };

	std::unordered_map<ComponentType, Ptr<Component>> m_Components;
};

template<>
struct Entity::TypeOf<TagComponent> { static ComponentType Get() { return ComponentType::Tag; } };

template<>
struct Entity::TypeOf<TextureComponent> { static ComponentType Get() { return ComponentType::Texture; } };

template<>
struct Entity::TypeOf<TransformComponent> { static ComponentType Get() { return ComponentType::Transform; } };

}