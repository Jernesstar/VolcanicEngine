#include "Geometry.h"

namespace Magma {

static PxShape* CreateBox();
static PxShape* CreateSphere();
static PxShape* CreatePlane();
// static PxShape* CreateCapsule();
static PxShape* CreateTriangleMesh(Ref<Mesh> mesh);
// static PxShape* CreateConvexMesh(Buffer<Vertex> data);

Shape Shape::Create(ShapeType type) {
	
}

Shape::Shape(Ref<Mesh> mesh)
	: Type(ShapeType::TriangleMesh)
{
	m_Shape = CreateTriangleMesh(mesh);
}

// Shape::Shape(Buffer<Vertex> data)
// 	: Type(ShapeType::ConvexShape)
// {
// 	m_Shape = CreateConvexShape(mesh);
// }

Shape::Shape(ShapeType type)
	: Type(type)
{
	switch(type) {
		case ShapeType::Box:
			m_Shape = CreateBox();
		case ShapeType::Sphere:
			m_Shape = CreateSphere();
		case ShapeType::Plane:
			m_Shape = CreatePlane();
		// case ShapeType::Capsule:
		// 	m_Shape = CreateCapsule();
		// case ShapeType::TriangleMesh:
		// 	m_Shape = CreateTriangleMesh();
		// case ShapeType::ConvexMesh:
		// 	m_Shape = CreateConvexMesh();
	}
}

Shape::~Shape() {
	if(m_Shape)
		m_Shape->release();
	
	m_Shape = nullptr;
}

PxShape* CreateBox() {
	auto material = s_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	return s_Physics->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *material);
}

PxShape* CreatSphere() {
	auto material = s_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	return s_Physics->createShape(PxSphereGeometry(0.5f), *material);
}

PxShape* CreatPlane() {
	auto material = s_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	return s_Physics->createShape(PxPlaneGeometry(), *material);
}

// PxShape* CreatCapsule() {
	// auto material = s_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	// return s_Physics->createShape(PxCapsuleGeometry(), *material);
// }

// PxShape* CreateConvexMesh(Buffer<Vertex> data) {
	// PxConvexMeshDesc desc;
	// desc.points.count  = 5;
	// desc.points.stride = sizeof(PxVec3);
	// desc.points.data   = verts;
	// desc.flags		   = PxConvexFlag::eCOMPUTE_CONVEX;

	// PxTolerancesScale scale;
	// PxCookingParams params(scale);
	// PxDefaultMemoryOutputStream buf;
	// PxConvexMeshCookingResult::Enum result;

	// bool success = PxCookConvexMesh(params, desc, buf, &result);
	// if(!success)
	// 	VOLCANICORE_LOG_ERROR("Failed to create convex mesh");

	// PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	// PxConvexMesh* mesh = s_Physics->createConvexMesh(input);
	// return s_Physics->createShape(PxConvexMeshGeometry(mesh), material, true);
// }

PxShape* CreateTriangleMesh(Ref<Mesh> mesh) {
	auto material = s_Physics->createMaterial(0.5f, 0.5f, 0.6f);

	PxTriangleMeshDesc desc;
	desc.points.count  = mesh->GetVertices().size();
	desc.points.stride = sizeof(Vertex);
	desc.points.data   = &mesh->GetVertices()[0];

	desc.triangles.count  = mesh->GetIndices().size();
	desc.triangles.stride = 3 * sizeof(uint32_t);
	desc.triangles.data   = &mesh->GetIndices()[0];

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	PxDefaultMemoryOutputStream buf;
	PxTriangleMeshCookingResult::Enum result;

	bool success = PxCookTriangleMesh(params, desc, buf, result);
	if(!success)
		VOLCANICORE_LOG_ERROR("Failed to create triangle mesh");

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxTriangleMesh* mesh = s_Physics->createTriangleMesh(input);
	return s_Physics->createShape(PxTriangleMeshGeometry(mesh), material, true);
}

}