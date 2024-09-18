#pragma once

#define PX_PHYSX_STATIC_LIB

// #include <PxPhysics.h>
// #include <PxPhysicsAPI.h>

// using namespace physx;

#include <VolcaniCore/Renderer/Transform.h>
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
		Mesh
	};

public:
	static Ref<Shape> Create(Shape::Type type);
	static Ref<Shape> Create(Ref<Mesh> mesh);
	static Ref<Shape> CreateBox(float radius);
	static Ref<Shape> CreateSphere(float radius);
	static Ref<Shape> CreatePlane(const Transform& tr);
	static Ref<Shape> CreateCapsule(float radius, float halfRadius);

public:
	Shape(Shape::Type type);
	Shape(const Shape& other);
	Shape& operator =(const Shape& other);
	~Shape();

	Shape::Type GetType() const { return m_Type; }

protected:
	Type m_Type;

	// PxShape* m_Shape;

	friend class RigidBody;
};

}