#pragma once

#include <VolcaniCore/Renderer/Transform.h>

#include "Shape.h"

using namespace physx;

namespace Magma::Physics {

class RigidBody {
public:
	enum class Type { Static, Dynamic };

public:
	static Ref<RigidBody> Create(RigidBody::Type type,
								 const Shape& shape,
								 const Transform& t = { });
	static Ref<RigidBody> Create(RigidBody::Type type,
								 const Transform& t = { });

public:
	RigidBody(RigidBody::Type type,
			  const Shape& shape,
			  const Transform& t = { });
	RigidBody(RigidBody::Type type,
			  const Transform& t = { })
	virtual ~RigidBody() = 0;

	RigidBody::Type GetType() { return m_Type; }
	Shape::Type GetShapeType() { return m_ShapeType; }

	void UpdateTransform();
	void UpdateTransform(const Transform& t);
	const Transform& GetTransform() const { return m_Transform; }

	void SetShape(const Shape& shape);
	bool HasShape() { return m_HasShape; }

	template<typename TDerived>
	requires std::derived_from<TDerived, RigidBody>
	TDerived* As() const { return (TDerived*)(this); }

protected:
	PxRigidActor* m_Actor;

	RigidBody::Type m_Type;
	Shape::Type m_ShapeType;
	bool m_HasShape = false;
	glm::mat4 m_Transform; // TODO: Remove this?

	friend class World;
};

class StaticBody : public RigidBody {
public:
	StaticBody(const Shape& shape, const Transform& t = { });
	StaticBody(const Transform& t = { });
	~StaticBody() = default;
};

class DynamicBody : public RigidBody {
public:
	DynamicBody(const Shape& shape, const Transform& t = { });
	DynamicBody(const Transform& t = { });
	~DynamicBody() = default;

	// void SetVelocity(const glm::vec3& velocity);

private:
	
};

}