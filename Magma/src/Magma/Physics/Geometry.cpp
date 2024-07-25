#include "Geometry.h"

namespace Magma {

static PxShape* CreateConvexMesh();
static PxShape* CreateTriangleMesh();

// Shape::Shape(ShapeType type, Args&&... args) {
// 	switch(type) {
// 		case Box:
// 			m_Shape = CreateBox(args);
// 		case ConvexMesh:
// 			m_Shape = CreateConvexMesh(args);
// 		case TriangleMesh:
// 			m_Shape = CreateTriangleMesh(args);
// 	}
// }

Shape::Shape() {

}

Shape::~Shape() {
	m_Shape->release();
}

PxShape* CreateConvexMesh() {
	// PxConvexMeshDesc desc;
	// desc.points.count  = 5;
	// desc.points.stride = sizeof(PxVec3);
	// desc.points.data   = verts;
	// desc.flags		   = PxConvexFlag::eCOMPUTE_CONVEX;

	// PxTolerancesScale scale;
	// PxCookingParams params(scale);
	// PxDefaultMemoryOutputStream buf;
	// PxConvexMeshCookingResult::Enum result;

	// bool status = PxCookConvexMesh(params, desc, buf, &result);
	// if(!status)
	// 	VOLCANICORE_LOG_ERROR("Failed to create convex mesh");

	// PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	// PxConvexMesh* mesh = gPhysics->createConvexMesh(input);
	// PxShape* convexShape =
	// 	gPhysics->createShape(PxConvexMeshGeometry(mesh), gMaterial, true);
}

PxShape* CreateTriangleMesh() {
	// PxTriangleMeshDesc desc;
	// desc.points.count  = mesh.GetVertices().size();
	// desc.points.stride = sizeof(Vertex);
	// desc.points.data   = &mesh.GetVertices()[0];

	// desc.triangles.count  = mesh.GetIndices().size();
	// desc.triangles.stride = 3 * sizeof(PxU32);
	// desc.triangles.data   = &mesh.GetIndices()[0];

	// PxTolerancesScale scale;
	// PxCookingParams params(scale);
	// PxDefaultMemoryOutputStream buf;
	// PxTriangleMeshCookingResult::Enum result;

	// bool status = PxCookTriangleMesh(params, desc, buf, result);
	// if(!status)
	// 	VOLCANICORE_LOG_ERROR("Failed to create triangle mesh");

	// PxDefaultMemoryInputData readBuffer(buf.getData(), buf.getSize());
	// PxTriangleMesh* mesh = gPhysics->createTriangleMesh(readBuffer);
	// PxShape* triangleShape = gPhysics->createShape(
	// 	PxTriangleMeshGeometry(mesh), gMaterial, true);
}


}