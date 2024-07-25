#pragma once

#include <VolcaniCore/Object/Mesh.h>

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