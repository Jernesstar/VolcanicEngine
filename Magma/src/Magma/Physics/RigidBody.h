#pragma once

#include <VolcaniCore/Graphics/Transform.h>

#include "Shape.h"

#ifdef MAGMA_PHYSICS
using namespace physx;
#endif

namespace Magma::Physics {

class RigidBody : public Derivable<RigidBody> {
public:
	enum class Type { Static, Dynamic };

	static Ref<RigidBody> Create(RigidBody::Type type,
								 Ref<Shape> shape = nullptr,
								 const Transform& t = { });

public:
	void* Data;

public:
	RigidBody(RigidBody::Type type, Ref<Shape> shape, const Transform& t = { });
	~RigidBody();

	void SetGravity(bool gravity);

#ifdef MAGMA_PHYSICS
	bool operator ==(const RigidBody& other) const {
		return m_Actor == other.m_Actor;
	}
	bool operator !=(const RigidBody& other) const {
		return m_Actor != other.m_Actor;
	}
#endif

	RigidBody::Type GetType() const { return m_Type; }

	virtual void SetShape(Ref<Shape> shape) = 0;
	Ref<Shape> GetShape() { return m_Shape; }
	bool HasShape() const { return m_Shape != nullptr; }

	void UpdateTransform();
	void SetTransform(const Transform& t);
	const Transform& GetTransform() const { return m_Transform; }

protected:
#ifdef MAGMA_PHYSICS
	PxRigidActor* m_Actor;
#endif

	RigidBody::Type m_Type;
	Transform m_Transform;

	Ref<Shape> m_Shape;

	friend class World;
};

class StaticBody : public RigidBody {
public:
	StaticBody(Ref<Shape> shape, const Transform& t = { });
	~StaticBody() = default;

	void SetShape(Ref<Shape> shape) override;
};

class DynamicBody : public RigidBody {
public:
	DynamicBody(Ref<Shape> shape, const Transform& t = { });
	~DynamicBody() = default;

	void SetShape(Ref<Shape> shape) override;

	void SetMass(float mass);
	void ApplyForce(const glm::vec3& f);
};

}