#include "Shape.h"

#include <VolcaniCore/Core/Log.h>

#include <Physics/Physics.h>

namespace Magma::Physics {

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

PxShape* CreateBox() {
	auto* material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	return GetPhysicsLib()
			->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *material);
}

PxShape* CreateSphere() {
	auto* material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	return GetPhysicsLib()
			->createShape(PxSphereGeometry(0.5f), *material);
}

PxShape* CreatePlane() {
	auto* material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	return GetPhysicsLib()
			->createShape(PxPlaneGeometry(), *material);
}

PxShape* CreatCapsule() {
	// auto material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	// return GetPhysicsLib()->createShape(PxCapsuleGeometry(), *material);
}

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
	// PxConvexMesh* mesh = GetPhysicsLib()->createConvexMesh(input);
	// return GetPhysicsLib()->createShape(PxConvexMeshGeometry(mesh), material);
// }

PxShape* CreateTriangleMesh(Ref<Mesh> mesh) {
	auto material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);

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

	bool success = PxCookTriangleMesh(params, desc, buf, &result);
	if(!success)
		VOLCANICORE_LOG_ERROR("Failed to create triangle mesh");

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxTriangleMesh* m = GetPhysicsLib()->createTriangleMesh(input);
	return GetPhysicsLib()->createShape(PxTriangleMeshGeometry(m), *material);
}

}