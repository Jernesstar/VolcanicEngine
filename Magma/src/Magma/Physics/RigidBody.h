#pragma once

#include <VolcaniCore/Renderer/Transform.h>

#include "Shape.h"

using namespace physx;

namespace Magma::Physics {

class RigidBody {
public:
	enum class Type { Static, Dynamic };

public:
	static Ref<RigidBody> Create(RigidBody::Type type, Ref<Shape> shape,
								 const Transform& t = { });

	static Ref<RigidBody> Create(RigidBody::Type type, const Transform& t = { });

public:
	RigidBody(RigidBody::Type type, Ref<Shape> shape,
			  const Transform& t = { });

	RigidBody(RigidBody::Type type, const Transform& t = { });
	~RigidBody();

	bool operator ==(const RigidBody& other) const {
		return m_Actor == other.m_Actor;
	}
	bool operator !=(const RigidBody& other) const {
		return m_Actor != other.m_Actor;
	}

	RigidBody::Type GetType() const { return m_Type; }

	virtual void SetShape(Ref<Shape> shape) = 0;
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

	Ref<Shape> m_Shape;

	friend class World;
};

class StaticBody : public RigidBody {
public:
	StaticBody(Ref<Shape> shape, const Transform& t = { });
	StaticBody(const Transform& t = { });
	~StaticBody() = default;

	void SetShape(Ref<Shape> shape) override;
};

class DynamicBody : public RigidBody {
public:
	DynamicBody(Ref<Shape> shape, const Transform& t = { });
	DynamicBody(const Transform& t = { });
	~DynamicBody() = default;

	// void SetVelocity(const glm::vec3& velocity);
	void SetShape(Ref<Shape> shape) override;

private:
	
};

}