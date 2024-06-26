#pragma once

#include <unordered_map>

#include "Component.h"

namespace Magma {

class Entity {
public:
	Entity() : m_ID() { }
	~Entity() = default;
	
	UUID GetID() { return m_ID; }
	operator uint32_t() const { return m_ID; }

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	bool Has() { return m_Components.count(TypeOf<TComponent>::Get()) == 1; }

	template<typename TComponent, typename ...Args>
	requires std::derived_from<TComponent, Component>
	TComponent& Add(Args&&... args) {
		if(Has<TComponent>()) return Get<TComponent>();
		m_Components[TypeOf<TComponent>::Get()]
			= new TComponent(std::forward<Args>(args)...);
		return *((TComponent*)m_Components[TypeOf<TComponent>::Get()]);
	}

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	TComponent& Get() {
		if(!Has<TComponent>()) return Add<TComponent>();
		return *((TComponent*)m_Components[TypeOf<TComponent>::Get()]);
	}

	template<typename TComponent>
	requires std::derived_from<TComponent, Component>
	void Remove() {
		if(!Has<TComponent>()) return;
		m_Components.erase(TypeOf<TComponent>::Get());
	}

	bool operator ==(const Entity& other) const { return this->m_ID == other.m_ID; }

private:
	UUID m_ID;
	std::unordered_map<ComponentType, Component*> m_Components;

	template<typename TComponent>
	struct TypeOf { static ComponentType Get() { return ComponentType::Unknown; } };
};

template<>
struct Entity::TypeOf<TagComponent> { 
	static ComponentType Get() { return ComponentType::Tag; }
};

template<>
struct Entity::TypeOf<TextureComponent> {
	static ComponentType Get() { return ComponentType::Texture; }
};

template<>
struct Entity::TypeOf<TransformComponent> {
	static ComponentType Get() { return ComponentType::Transform; }
};

}