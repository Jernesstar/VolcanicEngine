#include "Shape.h"

#include <VolcaniCore/Core/Log.h>
#include <VolcaniCore/Core/Buffer.h>

#include <Physics/Physics.h>

namespace Magma::Physics {

#ifdef MAGMA_PHYSICS
static PxShape* CookMesh(Ref<Mesh> mesh);
#endif

Ref<Shape> Shape::Create(Shape::Type type) {
	Ref<Shape> shape;
	switch(type) {
		case Shape::Type::Box:
			shape = CreateBox(0.5f);
			break;
		case Shape::Type::Sphere:
			shape = CreateSphere(0.5f);
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

#ifdef MAGMA_PHYSICS
	shape->m_Shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
#endif

	return shape;
}

Ref<Shape> Shape::Create(Ref<Mesh> mesh) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Mesh);
#ifdef MAGMA_PHYSICS
	auto* mat = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	shape->m_Shape = CookMesh(mesh);
#endif

	return shape;
}

Ref<Shape> Shape::CreateBox(float radius) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Box);
#ifdef MAGMA_PHYSICS
	auto* mat = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	shape->m_Shape =
		GetPhysicsLib()->createShape(
			PxBoxGeometry(radius, radius, radius), *mat);
#endif

	return shape;
}

Ref<Shape> Shape::CreateSphere(float radius) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Box);
#ifdef MAGMA_PHYSICS
	auto* material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	shape->m_Shape =
		GetPhysicsLib()->createShape(PxSphereGeometry(0.5f), *material);
#endif

	return shape;
}

Ref<Shape> Shape::CreatePlane(const Transform& tr) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Plane);
#ifdef MAGMA_PHYSICS
	auto* mat = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	shape->m_Shape =
		GetPhysicsLib()->createShape(PxPlaneGeometry(), *mat);
#endif
	return shape;
}

Ref<Shape> Shape::CreateCapsule(float radius, float halfRadius) {
	Ref<Shape> shape = CreateRef<Shape>(Shape::Type::Capsule);
#ifdef MAGMA_PHYSICS
	auto* mat = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);
	shape->m_Shape =
		GetPhysicsLib()->createShape(PxCapsuleGeometry(radius, halfRadius), *mat);
#endif

	return shape;
}

Shape::Shape(Shape::Type type)
	: m_Type(type) { }

Shape::Shape(const Shape& other)
	: m_Type(other.m_Type)
{
#ifdef MAGMA_PHYSICS
	m_Shape = other.m_Shape;
	m_Shape->acquireReference();
#endif
}

Shape& Shape::operator =(const Shape& other) {
	m_Type = other.m_Type;

#ifdef MAGMA_PHYSICS
	m_Shape = other.m_Shape;
	m_Shape->acquireReference();
#endif
	return *this;
}

Shape::~Shape() {
#ifdef MAGMA_PHYSICS
	m_Shape->release();

#endif
}

#ifdef MAGMA_PHYSICS

PxShape* CookConvex(Buffer<Vertex> data) {
	auto material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);

	PxConvexMeshDesc desc;
	desc.points.count  = 5;
	desc.points.stride = sizeof(Vertex);
	desc.points.data   = data.Get();
	desc.flags		   = PxConvexFlag::eCOMPUTE_CONVEX;

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;

	bool success = PxCookConvexMesh(params, desc, buf, &result);
	if(!success)
		VOLCANICORE_LOG_ERROR("Failed to create convex mesh");

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* mesh = GetPhysicsLib()->createConvexMesh(input);
	return GetPhysicsLib()->createShape(PxConvexMeshGeometry(mesh), *material);
}

PxShape* CookMesh(Ref<Mesh> mesh) {
	auto material = GetPhysicsLib()->createMaterial(0.5f, 0.5f, 0.6f);

	PxTriangleMeshDesc desc;
	desc.points.count  = mesh->SubMeshes[-1].Vertices.GetCount();
	desc.points.stride = sizeof(Vertex);
	desc.points.data   = mesh->SubMeshes[-1].Vertices.Get();

	desc.triangles.count  = mesh->SubMeshes[-1].Indices.GetCount() / 3;
	desc.triangles.stride = 3 * sizeof(uint32_t);
	desc.triangles.data   = mesh->SubMeshes[-1].Indices.Get();

	PxTolerancesScale scale;
	PxCookingParams params(scale);
	PxDefaultMemoryOutputStream buf;
	PxTriangleMeshCookingResult::Enum result;

	bool success = PxCookTriangleMesh(params, desc, buf, &result);
	if(!success) {
		VOLCANICORE_LOG_ERROR("Failed to create triangle mesh");
		return nullptr;
	}

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxTriangleMesh* m = GetPhysicsLib()->createTriangleMesh(input);
	return GetPhysicsLib()->createShape(PxTriangleMeshGeometry(m), *material);
}

#endif
}