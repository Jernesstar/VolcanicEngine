#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include <VolcaniCore/Object/Mesh.h>

using namespace physx;

namespace Magma {

enum class ShapeType { Box, Cylinder, Sphere, ConvexMesh, TriangleMesh };

class Shape {
public:
	// Shape(ShapeType primitive);
	Shape();
	~Shape();

private:
	PxShape* m_Shape;
};

}