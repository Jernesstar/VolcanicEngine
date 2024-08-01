#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

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
	// Shape(const Shape& other) = delete;
	// Shape& operator =(const Shape& other) = delete;
	~Shape();

private:
	PxShape* m_Shape;

	friend class RigidBody;
};

}