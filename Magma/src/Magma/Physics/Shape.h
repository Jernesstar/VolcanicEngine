#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Object/Mesh.h>

using namespace VolcaniCore;

namespace Magma::Physics {

enum class ShapeType { Box, Sphere, Plane, Capsule, ConvexMesh, TriangleMesh };

class Shape {
public:
	static Shape Create(ShapeType type);

public:
	const ShapeType Type;

public:
	Shape(ShapeType type);
	Shape(Ref<Mesh> mesh);
	// Shape(Buffer<Vertex> data);
	Shape(const Shape& other)
		: Type(other.Type)
	{
		m_Shape = other.m_Shape;
		m_Shape->acquireReference();
		VOLCANICORE_LOG_INFO("The copy constructor ran");
	}
	Shape& operator =(const Shape& other) {
		m_Shape = other.m_Shape;
		m_Shape->acquireReference();
		VOLCANICORE_LOG_INFO("The copy assigment operator ran");
		return *this;
	}
	~Shape();

private:
	PxShape* m_Shape;

	friend class RigidBody;
};

}