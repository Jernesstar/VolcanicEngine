#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

#include <VolcaniCore/Object/Mesh.h>

using namespace VolcaniCore;

namespace Magma::Physics {

class Shape {
public:
	enum class Type {
		Box,
		Sphere,
		Plane,
		Capsule,
		ConvexMesh,
		TriangleMesh
	};

public:
	static Shape Create(Shape::Type type);
	static Shape CreatePlan(const glm::vec3& transform);
	static Shape CreateCapsule(float radius, float halfRadius);
	static Shape CreateTriangleMesh(Ref<Mesh> mesh);

public:
	Shape(Shape::Type type);
	Shape(Ref<Mesh> mesh);
	// Shape(Buffer<Vertex> data);
	Shape(const Shape& other);
	Shape& operator =(const Shape& other);
	~Shape();

	Shape::Type GetType() const { return m_Type; }

private:
	Type m_Type;

	PxShape* m_Shape;

	friend class RigidBody;
	friend class StaticBody;
	friend class DynamicBody;
};

}