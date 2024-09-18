#include "Shape.h"

#include <VolcaniCore/Core/Log.h>

#include <Physics/Physics.h>

namespace Magma::Physics {

// static PxShape* CookMesh(Ref<Mesh> mesh);

Ref<Shape> Shape::Create(Shape::Type type) {
	Ref<Shape> shape;
	switch(type) {
		case Shape::Type::Box:
			shape = CreateBox(0.5f);
			break;
		case Shape::Type::Sphere:
			shape = CreateBox(0.5f);
			break;
		case Shape::Type::Plane:
			shape = CreatePlane(Transform{ });
			break;
		case Shape::Type::Capsule:
			shape = CreateCapsule(0.5f, 0.5f);
			break;
		case Shape::Type::Mesh:
			break;
	}

	return shape;
}

Ref<Shape> Shape::Create(Ref<Mesh> mesh) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Mesh);
	// auto* mat = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	// shape->m_Shape = CookMesh(mesh);

	return shape;
}

Ref<Shape> Shape::CreateBox(float radius) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Box);
	// auto* mat = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	// shape->m_Shape =
	// 	GetPhysicsLib()->createShape(PxBoxGeometry(0.5f, 0.5f, 0.5f), *mat);
	
	return shape;
}

Ref<Shape> Shape::CreateSphere(float radius) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Box);
	// auto* material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	// shape->m_Shape =
	// 	GetPhysicsLib()->createShape(PxSphereGeometry(0.5f), *material);

	return shape;
}

Ref<Shape> Shape::CreatePlane(const Transform& tr) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Plane);
	// auto* mat = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	// shape->m_Shape =
	// 	GetPhysicsLib()->createShape(PxPlaneGeometry(), *mat);

	return shape;
}

Ref<Shape> Shape::CreateCapsule(float radius, float halfRadius) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Capsule);
	// auto* mat = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	// shape->m_Shape =
	// 	GetPhysicsLib()->createShape(PxCapsuleGeometry(0.5f, 0.5f, 0.5f), *mat);

	return shape;
}

Shape::Shape(Shape::Type type)
	: m_Type(type) { }

Shape::Shape(const Shape& other)
	: m_Type(other.m_Type)
{
	// m_Shape = other.m_Shape;
	// m_Shape->acquireReference();
}

Shape& Shape::operator =(const Shape& other) {
	m_Type = other.m_Type;
	// m_Shape = other.m_Shape;
	// m_Shape->acquireReference();
	return *this;
}

Shape::~Shape() {
	// m_Shape->release();
}

// // PxShape* CookConvex(Buffer<Vertex> data) {
// 	// PxConvexMeshDesc desc;
// 	// desc.points.count  = 5;
// 	// desc.points.stride = sizeof(PxVec3);
// 	// desc.points.data   = verts;
// 	// desc.flags		   = PxConvexFlag::eCOMPUTE_CONVEX;

// 	// PxTolerancesScale scale;
// 	// PxCookingParams params(scale);
// 	// PxDefaultMemoryOutputStream buf;
// 	// PxConvexMeshCookingResult::Enum result;

// 	// bool success = PxCookConvexMesh(params, desc, buf, &result);
// 	// if(!success)
// 	// 	VOLCANICORE_LOG_ERROR("Failed to create convex mesh");

// 	// PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
// 	// PxConvexMesh* mesh = GetPhysicsLib()->createConvexMesh(input);
// 	// return GetPhysicsLib()->createShape(PxConvexMeshGeometry(mesh), material);
// // }

// PxShape* CookMesh(Ref<Mesh> mesh) {
// 	auto material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);

// 	PxTriangleMeshDesc desc;
// 	desc.points.count  = mesh->GetVertices().size();
// 	desc.points.stride = sizeof(Vertex);
// 	desc.points.data   = &mesh->GetVertices()[0];

// 	desc.triangles.count  = mesh->GetIndices().size();
// 	desc.triangles.stride = 3 * sizeof(uint32_t);
// 	desc.triangles.data   = &mesh->GetIndices()[0];

// 	PxTolerancesScale scale;
// 	PxCookingParams params(scale);
// 	PxDefaultMemoryOutputStream buf;
// 	PxTriangleMeshCookingResult::Enum result;

// 	bool success = PxCookTriangleMesh(params, desc, buf, &result);
// 	if(!success)
// 		VOLCANICORE_LOG_ERROR("Failed to create triangle mesh");

// 	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
// 	PxTriangleMesh* m = GetPhysicsLib()->createTriangleMesh(input);
// 	return GetPhysicsLib()->createShape(PxTriangleMeshGeometry(m), *material);
// }

}