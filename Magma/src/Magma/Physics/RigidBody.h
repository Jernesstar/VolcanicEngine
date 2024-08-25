#pragma once

#include <VolcaniCore/Renderer/Transform.h>

#include "Shape.h"

using namespace physx;

namespace Magma::Physics {

class RigidBody {
public:
	enum class Type { Static, Dynamic };

public:
	static Ref<RigidBody> Create(RigidBody::Type type, const Shape& shape,
								 const Transform& t = { });

	static Ref<RigidBody> Create(RigidBody::Type type, const Transform& t = { });

public:
	RigidBody(RigidBody::Type type, const Shape& shape,
			  const Transform& t = { });

	RigidBody(RigidBody::Type type, const Transform& t = { });
	~RigidBody();

	RigidBody& operator =(const RigidBody& other) {
		this->m_Actor = other.m_Actor;
		this->m_Actor->userData = this;

		this->m_Type = other.m_Type;
		// this->SetShape(other.m_Shape);

		return *this;
	}

	bool operator ==(const RigidBody& other) const {
		return m_Actor == other.m_Actor;
	}
	bool operator !=(const RigidBody& other) const {
		return m_Actor != other.m_Actor;
	}

	RigidBody::Type GetType() const { return m_Type; }

	virtual void SetShape(const Shape& shape) = 0;
	bool HasShape() const { return m_HasShape; }
	Shape::Type GetShapeType() const { return m_ShapeType; }

	void UpdateTransform();
	void UpdateTransform(const Transform& t);
	const Transform& GetTransform() const { return m_Transform; }

	template<typename TDerived>
	requires std::derived_from<TDerived, RigidBody>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	PxRigidActor* m_Actor;

	RigidBody::Type m_Type;
	Transform m_Transform;

	Shape::Type m_ShapeType;
	bool m_HasShape = false;
	// Ref<Shape> m_Shape;

	friend class World;
};

class StaticBody : public RigidBody {
public:
	StaticBody(const Shape& shape, const Transform& t = { });
	StaticBody(const Transform& t = { });
	~StaticBody() = default;

	void SetShape(const Shape& shape) override;
};

class DynamicBody : public RigidBody {
public:
	DynamicBody(const Shape& shape, const Transform& t = { });
	DynamicBody(const Transform& t = { });
	~DynamicBody() = default;

	// void SetVelocity(const glm::vec3& velocity);
	void SetShape(const Shape& shape) override;

private:
	
};

}