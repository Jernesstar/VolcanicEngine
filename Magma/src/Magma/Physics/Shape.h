#pragma once

#define PX_PHYSX_STATIC_LIB

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
	static Ref<Shape> Create(Shape::Type type);
	static Ref<Shape> CreateBox(float radius);
	static Ref<Shape> CreatePlane(const Transform& tr);
	static Ref<Shape> CreateCapsule(float radius, float halfRadius);
	static Ref<Shape> CreateMesh(Ref<Mesh> mesh);

public:
	Shape(Shape::Type type);
	Shape(const Shape& other);
	Shape& operator =(const Shape& other);
	~Shape();

	Shape::Type GetType() const { return m_Type; }

protected:
	Type m_Type;

	PxShape* m_Shape;

	friend class RigidBody;
};

}